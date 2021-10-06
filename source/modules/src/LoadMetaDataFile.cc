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
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>


using namespace anlnext;

namespace comptonsoft {

LoadMetaDataFile::LoadMetaDataFile()
  : dataReaderModule_("LoadReducedFrame"),
    dataFileExtension_(".root"),
    metaFileExtension_(".json")
{
}

ANLStatus LoadMetaDataFile::mod_define()
{
  define_parameter("data_reader_module", &mod_class::dataReaderModule_);
  define_parameter("datafile_extension", &mod_class::dataFileExtension_);
  define_parameter("metafile_extension", &mod_class::metaFileExtension_);
  
  return AS_OK;
}

ANLStatus LoadMetaDataFile::mod_initialize()
{
  get_module_IFNC(dataReaderModule_, &data_reader_);

  return AS_OK;
}

ANLStatus LoadMetaDataFile::mod_analyze()
{
  datafilename_ = data_reader_->Filename();

  std::string metafilename = make_metafilename(datafilename_);
  if (metafilename.size() == 0) {
    std::cout << "DataFile " << datafilename_ << " does not have extension : " << dataFileExtension_ << std::endl;
    return AS_QUIT;
  }

  load_json(metafilename);

  return AS_OK;
}

std::string LoadMetaDataFile::make_metafilename(std::string datafilename)
{
  size_t filename_size = datafilename.size();
  size_t extension_size = dataFileExtension_.size();
  if (filename_size < extension_size || datafilename.substr(filename_size-extension_size) != dataFileExtension_) {
    return "";
  }
  return datafilename.substr(0, filename_size-extension_size) + metaFileExtension_;
}

void LoadMetaDataFile::load_json(std::string filename)
{
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(filename, pt);

  temperature_ = pt.get_optional<int>("temperature").get();
  capture_time_ = convert_datetime(pt.get_optional<std::string>("time").get(), "%Y-%m-%dT%H:%M:%S");
}

std::chrono::system_clock::time_point LoadMetaDataFile::convert_datetime(std::string datetime, std::string format)
{
  std::tm tm = {};
  std::stringstream ss(datetime);
  ss >> std::get_time(&tm, format.c_str());
  std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

  return tp;
}

} /* namespace comptonsoft */
