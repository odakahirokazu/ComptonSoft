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

#include "ExtractXrayEventImageFromQuickLookDB.hh"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "XrayEventCollection.hh"

using namespace anlnext;
using namespace mongocxx;
using namespace bsoncxx::builder::basic;

namespace comptonsoft {

ExtractXrayEventImageFromQuickLookDB::ExtractXrayEventImageFromQuickLookDB()
  : initial_frame_id_(0),
    max_frame_per_loop_(10),
    HostName_("127.0.0.1")
{
}

ANLStatus ExtractXrayEventImageFromQuickLookDB::mod_define()
{
  ExtractXrayEventImage::mod_define();

  define_parameter("host_name", &mod_class::HostName_);
  define_parameter("db_name", &mod_class::dbName_);
  define_parameter("collection_name", &mod_class::collectionName_);
  define_parameter("collection_type", &mod_class::collectionType_);
  define_parameter("initial_frame_id", &mod_class::initial_frame_id_);
  define_parameter("max_frame_per_loop", &mod_class::max_frame_per_loop_);
  define_parameter("analysis_id", &mod_class::analysisId_);
  
  return AS_OK;
}

ANLStatus ExtractXrayEventImageFromQuickLookDB::mod_initialize()
{
  ANLStatus status = ExtractXrayEventImage::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }

  last_frame_id_ = initial_frame_id_-1;
  if (analysisId_.empty()) {
    setLatestAnalysisId();
  }

  return AS_OK;
}

ANLStatus ExtractXrayEventImageFromQuickLookDB::mod_analyze()
{
  XrayEventCollection* event_collection = getEventCollection();

  if (collectionType_ == 0) {
    mongocxx::pipeline p{};
    p.match(make_document(kvp("analysis_id", analysisId_)));
    p.group(make_document(
      kvp("_id", make_document(kvp("ix", "$ix"), kvp("iy", "$iy"))),
      kvp("count", make_document(kvp("$sum", 1)))));

    const mongocxx::client conn{mongocxx::uri{}};
    auto db_ = conn[dbName_];
    auto cursor = db_[collectionName_].aggregate(p, mongocxx::options::aggregate{});
    for (auto&& elem : cursor) {
      const int ix = elem["_id"]["ix"].get_int32();
      const int iy = elem["_id"]["iy"].get_int32();
      const int count = elem["count"].get_int32();
      for (int k=0; k<count; k++) {
        XrayEvent_sptr event(new XrayEvent(1));
        event->setPixelX(ix);
        event->setPixelY(iy);
        event_collection->insertEvent(event);
      }
    }
  }
  else {
    using bsoncxx::builder::stream::open_document;
    using bsoncxx::builder::stream::close_document;

    const mongocxx::client conn{mongocxx::uri{}};
    auto db = conn[dbName_];
    mongocxx::cursor cursor = db[collectionName_].find(
      bsoncxx::builder::stream::document{}
        << "analysis_id" << analysisId_
        << "frameID" << open_document
          << "$gt" << last_frame_id_
          << "$lte" << last_frame_id_ + max_frame_per_loop_
        << close_document << bsoncxx::builder::stream::finalize
    );

    for (auto&& elem : cursor) {
      const int frame_id = elem["frameID"].get_int32();
      std::cout << "[ExtractXrayEventImageFromQuickLookDB] frameID : " << frame_id << std::endl;
      if (frame_id > last_frame_id_) {
        last_frame_id_ = frame_id;
      }
      auto xs = elem["x_list"];
      auto ys = elem["y_list"];
      const int size = elem["size"].get_int32();
      for (int i=0; i<size; i++) {
        const int ix = xs[i].get_int32();
        const int iy = ys[i].get_int32();
        XrayEvent_sptr event(new XrayEvent(1));
        event->setPixelX(ix);
        event->setPixelY(iy);
        event_collection->insertEvent(event);
      }
    }
  }

  return ExtractXrayEventImage::mod_analyze();
}

void ExtractXrayEventImageFromQuickLookDB::setLatestAnalysisId()
{
  const mongocxx::client conn{mongocxx::uri{}};
  auto db_ = conn[dbName_];

  mongocxx::pipeline p{};
  p.sort(make_document(kvp("analysis_id", -1)));
  p.limit(1);

  auto cursor = db_[collectionName_].aggregate(p, mongocxx::options::aggregate{});

  analysisId_ = (*cursor.begin())["analysis_id"].get_utf8().value.to_string();
}

} /* namespace comptonsoft */
