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

#include "LoadMetaDataFile.hh"
#include <iostream>
#include <chrono>
#include <sstream>
#include <ctime>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

using namespace anlnext;

namespace {

std::chrono::system_clock::time_point convert_datetime(const std::string& datetime, const std::string& format)
{
  std::tm tm = {};
  std::stringstream ss(datetime);
  ss >> std::get_time(&tm, format.c_str());
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  return tp;
}

} /* anonymous namespace */


namespace comptonsoft {

LoadMetaDataFile::LoadMetaDataFile()
  : data_reader_module_("LoadReducedFrame"),
    data_file_extension_(".root"),
    meta_file_extension_(".json")
{
}

ANLStatus LoadMetaDataFile::mod_define()
{
  define_parameter("data_reader_module", &mod_class::data_reader_module_);
  define_parameter("datafile_extension", &mod_class::data_file_extension_);
  define_parameter("metafile_extension", &mod_class::meta_file_extension_);
  
  return AS_OK;
}

ANLStatus LoadMetaDataFile::mod_initialize()
{
  get_module_IFNC(data_reader_module_, &data_reader_);

  return AS_OK;
}

ANLStatus LoadMetaDataFile::mod_analyze()
{
  data_filename_ = data_reader_->CurrentFilename();

  std::string filename = boost::filesystem::path(data_filename_).replace_extension(meta_file_extension_).string();
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(filename, pt);
  temperature_ = pt.get_optional<int>("temperature").get();
  capture_time_ = convert_datetime(pt.get_optional<std::string>("time").get(), "%Y-%m-%dT%H:%M:%S");

  return AS_OK;
}

} /* namespace comptonsoft */
