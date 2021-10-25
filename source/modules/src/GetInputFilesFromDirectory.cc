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

#include "GetInputFilesFromDirectory.hh"
#include <thread>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "LoadFrame.hh"

using namespace anlnext;

namespace comptonsoft {

GetInputFilesFromDirectory::GetInputFilesFromDirectory()
  : reader_module_("LoadFrame"),
    directory_("."),
    extension_(".dat"),
    delay_(5),
    wait_(10)
{
}

ANLStatus GetInputFilesFromDirectory::mod_define()
{
  define_parameter("reader_module", &mod_class::reader_module_);
  define_parameter("directory", &mod_class::directory_);
  define_parameter("extension", &mod_class::extension_);
  define_parameter("delay", &mod_class::delay_);
  define_parameter("wait", &mod_class::wait_);
  define_parameter("directories", &mod_class::directories_);
  
  return AS_OK;
}

ANLStatus GetInputFilesFromDirectory::mod_initialize()
{
  get_module_IFNC(reader_module_, &data_reader_);
  entry_time_ = std::time(nullptr);
  if (directories_.size()==0) {
    directories_.push_back(directory_);
  }

  return AS_OK;
}

ANLStatus GetInputFilesFromDirectory::mod_analyze()
{
  namespace fs = boost::filesystem;

  if (!redoing_ && directory_index_==static_cast<int>(directories_.size())) {
    return AS_OK;
  }

  if (!fs::exists(directories_[directory_index_])) {
    directory_index_++;
    directory_index_ %= static_cast<int>(directories_.size());
    return AS_OK;
  }

  if (redoing_) {
    redoing_ = false;
  }
  else {
    entry_time_ = std::time(nullptr);
  }

  const fs::path dir(directories_[directory_index_]);
  
  std::list<fs::path> files;
  for (const auto& f: boost::make_iterator_range(fs::directory_iterator(dir), {})) {
    if (fs::is_directory(f)) { continue; }
    
    const fs::path file = f.path();
    
    if (file.extension() != extension_) { continue; }
    if (data_reader_->hasFile(file.string())) { continue; }
    
    files.push_back(file);
    last_entry_index_ = directory_index_;
  }

  files.sort();
  bool added = false;
  for (const fs::path& file: files) {
    const std::time_t timeModified = fs::last_write_time(file);
    if (timeModified+delay_ < std::time(nullptr)) {
      data_reader_->addFile(file.string());
      added = true;
    }
  }
  
  if (!added && data_reader_->isDone()) {
    if (entry_time_+wait_ < std::time(nullptr)) {
      std::cout << "[GetInputFilesFromDirectory] timeout" << std::endl;
        directory_index_++;
        directory_index_ %= static_cast<int>(directories_.size());
    }
    else {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      redoing_ = true;
      return AS_REDO;
    }
  }

  
  return AS_OK;
}

} /* namespace comptonsoft */
