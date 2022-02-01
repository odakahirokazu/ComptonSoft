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

#ifndef COMPTONSOFT_LoadMetaDataFile_H
#define COMPTONSOFT_LoadMetaDataFile_H 1

#include <chrono>
#include <anlnext/BasicModule.hh>
#include "LoadReducedFrame.hh"
#include "VDataReader.hh"

namespace comptonsoft {

/**
 * LoadReducedFrame
 * 
 * @author Taihei Watanabe
 * @date 2021-09-30
 * @date 2022-02-01 | 1.2 | Hirokazu Odaka | code review
 */
class LoadMetaDataFile : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(LoadMetaDataFile, 1.2);

public:
  LoadMetaDataFile();
  
protected:
  LoadMetaDataFile(const LoadMetaDataFile&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  int Temperature() { return temperature_; };
  std::chrono::system_clock::time_point CaptureTime() { return capture_time_; };
  std::string Filename() { return data_filename_; };

private:
  std::string data_reader_module_;
  std::string data_file_extension_;
  std::string meta_file_extension_;

  VDataReader* data_reader_ = nullptr;

  std::string data_filename_ = "";
  int temperature_ = 0;
  std::chrono::system_clock::time_point capture_time_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_LoadMetaDataFile_H */
