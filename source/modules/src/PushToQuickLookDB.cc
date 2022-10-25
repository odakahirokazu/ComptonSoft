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

#include "PushToQuickLookDB.hh"

#include <fstream>
#include <boost/filesystem.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "hsquicklook/MongoDBClient.hh"
#include "hsquicklook/DocumentBuilder.hh"

using namespace anlnext;

namespace comptonsoft {

PushToQuickLookDB::PushToQuickLookDB()
  : canvas_width_(480), canvas_height_(480),
    collection_("analysis"),
    directory_("basic"), document_("images")
{
}

ANLStatus PushToQuickLookDB::mod_define()
{
  define_parameter("canvas_width", &mod_class::canvas_width_);
  define_parameter("canvas_height", &mod_class::canvas_height_);
  define_parameter("module_list", &mod_class::moduleList_);
  define_parameter("collection", &mod_class::collection_);
  define_parameter("directory", &mod_class::directory_);
  define_parameter("document", &mod_class::document_);
  define_parameter("period", &mod_class::period_);
  define_parameter("phase", &mod_class::phase_);
  
  return AS_OK;
}    

ANLStatus PushToQuickLookDB::mod_initialize()
{
  const int num_modules = moduleList_.size();
  modules_.resize(num_modules, nullptr);
  for (int i=0; i<num_modules; i++) {
    get_module_NC(moduleList_[i], &modules_[i]);
  }
  if (exist_module("MongoDBClient")) {
    get_module_NC("MongoDBClient", &mongodb_);
    mongodb_->createCappedCollection(collection_, 100*1024*1024);
  }

  const std::string canvasName = module_id() + "_canvas";
  canvas_ = new TCanvas(canvasName.c_str(),
                        canvasName.c_str(),
                        canvas_width_+4,
                        canvas_height_+24);

  return AS_OK;
}

ANLStatus PushToQuickLookDB::mod_analyze()
{
  if (get_loop_index()%period_ != phase_) {
    return AS_OK;
  }
  
  for (auto& module: modules_) {
    module->drawCanvas(canvas_, &fileList_);
  }

  if (mongodb_) {
    pushImagesToDB();
  }

  return AS_OK;
}

ANLStatus PushToQuickLookDB::mod_end_run()
{
  for (auto& module: modules_) {
    module->drawCanvas(canvas_, &fileList_);
  }

  if (mongodb_) {
    pushImagesToDB();
  }

  return AS_OK;
}

void PushToQuickLookDB::pushImagesToDB()
{
  const std::size_t size = 10*1024*1024;
  static uint8_t buf[size];

  hsquicklook::DocumentBuilder builder(directory_, document_);
  builder.setTimeNow();

  const std::string block_name = module_id();
  auto block_open = bsoncxx::builder::stream::document{};
  block_open << "Index" << static_cast<int64_t>(get_loop_index());
  for (const std::string& filename: fileList_) {
    std::ifstream fin(filename.c_str(), std::ios::in|std::ios::binary);
    fin.read((char*)buf, size);
    const std::size_t readSize = fin.gcount();
    fin.close();

    const boost::filesystem::path filePath(filename);
    block_open
      << filePath.stem().string() << hsquicklook::make_image_value(buf,
                                                                   readSize,
                                                                   canvas_width_,
                                                                   canvas_height_,
                                                                   filename);
  }
  
  auto block = block_open << bsoncxx::builder::stream::finalize;
  builder.addSection(block_name, block);

  auto doc = builder.generate();
  mongodb_->push(collection_, doc);
}

} /* namespace comptonsoft */
