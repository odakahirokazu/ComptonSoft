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

#include "PushXrayEventToQuickLookDB.hh"
#include "AnalyzeFrame.hh"
#include "LoadMetaDataFile.hh"

#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include "hsquicklook/MongoDBClient.hh"
#include "hsquicklook/DocumentBuilder.hh"

using namespace anlnext;

namespace comptonsoft {

PushXrayEventToQuickLookDB::PushXrayEventToQuickLookDB()
  : collection_("analysis")
{
}

ANLStatus PushXrayEventToQuickLookDB::mod_define()
{
  define_parameter("collection", &mod_class::collection_);
  define_parameter("period", &mod_class::period_);
  define_parameter("phase", &mod_class::phase_);
  define_parameter("collection_max_size", &mod_class::collection_max_size_);
  define_parameter("analysis_id", &mod_class::analysis_id_);
  
  return AS_OK;
}    

ANLStatus PushXrayEventToQuickLookDB::mod_initialize()
{
  get_module_NC("AnalyzeFrame", &analyzeFrame_);
  get_module_NC("LoadMetaDataFile", &metaDataFile_);

  if (exist_module("MongoDBClient")) {
    get_module_NC("MongoDBClient", &mongodb_);
    mongodb_->createCollection(collection_);
  }

  return AS_OK;
}

ANLStatus PushXrayEventToQuickLookDB::mod_analyze()
{
  if (get_loop_index()%period_ != phase_) {
    return AS_OK;
  }

  if (mongodb_) {
    es_ = analyzeFrame_->Events();
    pushXrayEventsToDB();
  }

  return AS_OK;
}

void PushXrayEventToQuickLookDB::pushXrayEventsToDB()
{
  std::vector<bsoncxx::document::value> documents;

  for (auto& event : es_) {
    documents.push_back(
      bsoncxx::builder::stream::document{}
        << "analysis_id" << analysis_id_
        << "frameID" << event->FrameID()
        << "ix" << event->PixelX()
        << "iy" << event->PixelY()
        << "sumPH" << event->SumPH()
        << "centerPH" << event->CenterPH()
        << "angle" << event->Angle()
        << "weight" << event->Weight()
        << "rank" << event->Rank()
        << "temperature" << metaDataFile_->Temperature()
        << "capture_time" << bsoncxx::types::b_date(metaDataFile_->CaptureTime())
        << "filename" << metaDataFile_->Filename()
      << bsoncxx::builder::stream::finalize
    );
  }
  mongodb_->push_many(collection_, documents);
}

} /* namespace comptonsoft */
