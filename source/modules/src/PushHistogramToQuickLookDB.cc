/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#include "PushHistogramToQuickLookDB.hh"
#include <vector>
#include <cmath>
#include <boost/format.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include "hsquicklook/MongoDBClient.hh"
#include "hsquicklook/DocumentBuilder.hh"

using namespace anlnext;

using namespace mongocxx;
using namespace bsoncxx::builder::basic;

namespace comptonsoft {

PushHistogramToQuickLookDB::PushHistogramToQuickLookDB()
  : event_collection_module_name_("XrayEventCollection"),
    recent_collection_name_("recents"),
    profile_collection_name_("profiles")
{
}

ANLStatus PushHistogramToQuickLookDB::mod_define()
{
  define_parameter("event_collection_module_name", &mod_class::event_collection_module_name_);
  define_parameter("collection_recent", &mod_class::recent_collection_name_);
  define_parameter("collection_profile", &mod_class::profile_collection_name_);
  define_parameter("directory", &mod_class::directory_);
  define_parameter("analysis_id", &mod_class::analysis_id_);
  define_parameter("columns_name", &mod_class::columns_name_);
  define_parameter("columns_min", &mod_class::columns_min_);
  define_parameter("columns_max", &mod_class::columns_max_);
  define_parameter("columns_bin", &mod_class::columns_bin_);
  
  return AS_OK;
}    

ANLStatus PushHistogramToQuickLookDB::mod_initialize()
{
  get_module_NC(event_collection_module_name_, &event_collection_module_);
  get_module_NC("LoadMetaDataFile", &metadata_file_module_);

  if (exist_module("MongoDBClient")) {
    get_module_NC("MongoDBClient", &mongodb_);
    mongodb_->createCappedCollection(recent_collection_name_, 100*1024*1024);
    mongodb_->createCollection(profile_collection_name_);
  }
  initializeHistograms();
  loadHistogramsFromMongoDB();

  return AS_OK;
}

ANLStatus PushHistogramToQuickLookDB::mod_analyze()
{
  if (mongodb_) {
    updateHistograms();
    updateTrends();
    updateMFparameters();
    pushRecentHistograms();
    pushProfile();
  }

  return AS_OK;
}

void PushHistogramToQuickLookDB::pushRecentHistograms()
{
  using bsoncxx::builder::stream::open_array;
  using bsoncxx::builder::stream::close_array;
  using bsoncxx::builder::stream::open_document;
  using bsoncxx::builder::stream::close_document;

  bsoncxx::builder::stream::document builder{};

  // histograms
  auto builder_opened = builder << "analysis_id" << analysis_id_
    << "directory" << directory_;
  for (size_t i=0; i<columns_name_.size(); i++) {
    std::string col = columns_name_[i];
    auto in_doc = builder_opened << col << open_document
      << "type" << "histogram"
      << "min" << columns_min_[i]
      << "max" << columns_max_[i]
      << "bin" << columns_bin_[i]
      << "value" << open_array;
    for (int j=0; j<columns_bin_[i]; j++) {
      in_doc << histograms_[i][j];
    }
    builder_opened = in_doc << close_array << close_document;
  }

  // trends
  auto doc = builder_opened << "frameID" << open_document
    << "type" << "trend"
    << "size" << (int)(count_trend_.size())
    << "value" << open_array;
  for (size_t j=0; j<count_trend_.size(); j++) {
    doc << count_trend_[j];
  }
  builder_opened = doc << close_array << close_document;

  doc = builder_opened << "temperature" << open_document
    << "type" << "trend"
    << "size" << (int)(temperature_trend_.size())
    << "value" << open_array;
  for (size_t j=0; j<temperature_trend_.size(); j++) {
    doc << temperature_trend_[j];
  }
  builder_opened = doc << close_array << close_document;

  // scalars
  builder_opened = builder_opened 
    << "capture_time" << open_document
    << "type" << "scalar"
    << "value" << bsoncxx::types::b_date(metadata_file_module_->CaptureTime())
    << close_document
    << "filename" << open_document
    << "type" << "scalar"
    << "value" << metadata_file_module_->Filename()
    << close_document
    << "modulation_factor" << open_document
    << "type" << "scalar"
    << "value" << mf_output_
    << close_document
    << "nh" << open_document
    << "type" << "scalar"
    << "value" << Nh_
    << close_document
    << "nv" << open_document
    << "type" << "scalar"
    << "value" << Nv_
    << close_document;

  mongodb_->push(recent_collection_name_, builder_opened << bsoncxx::builder::stream::finalize);
}

void PushHistogramToQuickLookDB::pushProfile()
{
  using bsoncxx::builder::stream::open_array;
  using bsoncxx::builder::stream::close_array;

  const XrayEventContainer& events = event_collection_module_->getEvents();
  
  bsoncxx::builder::stream::document builder{};
  auto builder_opened = builder
    << "analysis_id" << analysis_id_
    << "directory" << directory_
    << "frameID" << static_cast<int>(count_trend_.size())-1
    << "size" << static_cast<int>(events.size())
    << "x_list" << open_array;
  for (const auto& event : events) {
    builder_opened << event->PixelX();
  }
  auto doc = builder_opened << close_array;
  builder_opened = doc << "y_list" << open_array;
  for (const auto& event : events) {
    builder_opened << event->PixelY();
  }
  doc = builder_opened << close_array;

  mongodb_->push(profile_collection_name_, doc << bsoncxx::builder::stream::finalize);
}

void PushHistogramToQuickLookDB::initializeHistograms()
{
  histograms_.resize(columns_name_.size());
  for (size_t i=0; i<columns_name_.size(); i++){
    histograms_[i].resize(columns_bin_[i], 0);
  }
}

void PushHistogramToQuickLookDB::loadHistogramsFromMongoDB()
{
  bsoncxx::builder::stream::document builder;

  mongocxx::pipeline p{};
  p.match(make_document(kvp("analysis_id", analysis_id_)));
  p.sort(make_document(kvp("frameID", -1)));
  p.limit(1);

  auto cursor = mongodb_->aggregate(recent_collection_name_, p);

  for (auto&& doc : cursor) {
    for (size_t i=0; i<columns_name_.size(); i++) {
      const std::string column = columns_name_[i];
      for (int j=0; j<columns_bin_[i]; j++) {
        histograms_[i][j] = doc[column]["value"][j].get_int32();
      }
    }
    const int size = doc["frameID"]["size"].get_int32();
    for (int j=0; j<size; j++) {
      count_trend_.push_back(doc["frameID"]["value"][j].get_int32());
      temperature_trend_.push_back(doc["temperature"]["value"][j].get_int32());
    }
    Nh_ = doc["nh"]["value"].get_int32();
    Nv_ = doc["nv"]["value"].get_int32();
  }
}

void PushHistogramToQuickLookDB::updateHistograms()
{
  const XrayEventContainer& events = event_collection_module_->getEvents();
  for (size_t i=0; i<columns_name_.size(); i++) {
    const std::string col = columns_name_[i];
    for (const auto& event : events) {
      double val = columns_min_[i] - 1.0;
      if (col == "sumPH") {
        val = event->SumPH();
      } else if (col == "ix") {
        val = event->PixelX();
      } else if (col == "iy") {
        val = event->PixelY();
      } else if (col == "weight") {
        val = event->Weight();
      } else {
        break;
      }
      const int ind = (val - columns_min_[i]) * (double)columns_bin_[i] / (columns_max_[i] - columns_min_[i]);
      if (0 <= ind && ind < static_cast<int>(histograms_[i].size())) {
        histograms_[i][ind]++;
      }
    }
  }
}

void PushHistogramToQuickLookDB::updateTrends()
{
  const XrayEventContainer& events = event_collection_module_->getEvents();
  count_trend_.push_back(events.size());
  temperature_trend_.push_back(metadata_file_module_->Temperature());
}

void PushHistogramToQuickLookDB::updateMFparameters()
{
  const XrayEventContainer& events = event_collection_module_->getEvents();
  for (const auto& event : events) {
    if (event->Weight() == 2 && event->Rank() == 1) {
      const double deg = event->Angle();
      if (std::abs(deg - M_PI) < epi || std::abs(deg) < epi) {
        Nh_++;
      } else if (std::abs(deg - M_PI/2) < epi || std::abs(deg + M_PI/2) < epi) {
        Nv_++;
      }
    }
  }
  if (Nh_+Nv_ > 0) {
    const double modulation_factor = (double)(Nh_-Nv_) / (double)(Nh_+Nv_);
    const double modulation_factor_error = modulation_factor / std::sqrt(Nh_+Nv_);
    mf_output_ = (boost::format("%.4f +- %.4f") % modulation_factor % modulation_factor_error).str();
  }
}

} /* namespace comptonsoft */
