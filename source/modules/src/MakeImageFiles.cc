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

#include "MakeImageFiles.hh"

#include <fstream>
#include <boost/filesystem.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include "hsquicklook/MongoDBClient.hh"
#include "hsquicklook/DocumentBuilder.hh"

using namespace anlnext;

namespace comptonsoft {

MakeImageFiles::MakeImageFiles()
{
}

ANLStatus MakeImageFiles::mod_define()
{
  define_parameter("module_list", &mod_class::moduleList_);
  
  return AS_OK;
}    

ANLStatus MakeImageFiles::mod_initialize()
{
  const int num_modules = moduleList_.size();
  modules_.resize(num_modules, nullptr);
  for (int i=0; i<num_modules; i++) {
    get_module_NC(moduleList_[i], &modules_[i]);
  }

  if (exist_module("MongoDBClient")) {
    get_module_NC("MongoDBClient", &mongodb_);
    mongodb_->createCappedCollection("images", 100*1024*1024);
  }

  return AS_OK;
}

ANLStatus MakeImageFiles::mod_end_run()
{
  const int num_modules = moduleList_.size();
  canvas_ = new TCanvas("c1", "c1", 604, 628);
  for (int i=0; i<num_modules; i++) {
    modules_[i]->drawOutputFiles(canvas_, &fileList_);
  }

  if (mongodb_) {
    pushImagesToDB();
  }

  return AS_OK;
}

void MakeImageFiles::pushImagesToDB()
{
  const std::size_t size = 10*1024*1024;
  static uint8_t buf[size];

  hsquicklook::DocumentBuilder builder("Analysis", "Images");
  builder.setTimeNow();

  const std::string block_name = "MakeImageFiles";
  auto block_open = bsoncxx::builder::stream::document{};
  block_open << "Index" << static_cast<int64_t>(get_loop_index());
  for (const std::string& filename: fileList_) {
    std::ifstream fin(filename.c_str(), std::ios::in|std::ios::binary);
    fin.read((char*)buf, size);
    const std::size_t readSize = fin.gcount();
    fin.close();

    constexpr int ImageWidth = 600;
    constexpr int ImageHeight = 600;
    const boost::filesystem::path filePath(filename);
    block_open
      << filePath.stem().string() << hsquicklook::make_image_value(buf,
                                                                   readSize,
                                                                   ImageWidth,
                                                                   ImageHeight,
                                                                   filename);
  }
  
  auto block = block_open << bsoncxx::builder::stream::finalize;
  builder.addBlock(block_name, block);

  auto doc = builder.generate();
  mongodb_->push("images", doc);
}

} /* namespace comptonsoft */
