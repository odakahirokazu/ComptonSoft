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

#include "GetInputFiles.hh"
#include <ctime>
#include <thread>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "LoadFrame.hh"

using namespace anlnext;

namespace comptonsoft {

GetInputFiles::GetInputFiles()
  : directory_("."), extension_(".dat"), delay_(5), wait_(10)
{
}

ANLStatus GetInputFiles::mod_define()
{
  define_parameter("directory", &mod_class::directory_);
  define_parameter("extension", &mod_class::extension_);
  define_parameter("delay", &mod_class::delay_);
  define_parameter("wait", &mod_class::wait_);
  
  return AS_OK;
}

ANLStatus GetInputFiles::mod_initialize()
{
  get_module_NC("LoadFrame", &data_reader_);

  return AS_OK;
}

ANLStatus GetInputFiles::mod_analyze()
{
  namespace fs = boost::filesystem;

  const std::time_t entryTime = std::time(nullptr);
  const fs::path dir(directory_);
  
  while (true) {
    std::list<fs::path> files;
    
    for (const auto& f: boost::make_iterator_range(fs::directory_iterator(dir), {})) {
      if (fs::is_directory(f)) { continue; }

      const fs::path file = f.path();

      if (file.extension() != extension_) { continue; }
      if (data_reader_->hasFile(file.string())) { continue; }

      files.push_back(file);
    }

    if (files.empty()) {
      if (entryTime+wait_ < std::time(nullptr)) {
        break;
      }
    }

    files.sort();
    for (const fs::path& file: files) {
      const std::time_t timeModified = fs::last_write_time(file);
      if (timeModified+delay_ < std::time(nullptr)) {
        data_reader_->addFile(file.string());
      }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return AS_OK;
}

} /* namespace comptonsoft */
