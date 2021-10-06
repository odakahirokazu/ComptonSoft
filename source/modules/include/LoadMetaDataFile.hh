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

class LoadMetaDataFile : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(LoadMetaDataFile, 1.0);

public:
  LoadMetaDataFile();
  
protected:
  LoadMetaDataFile(const LoadMetaDataFile&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  std::string make_metafilename(std::string datafilename);
  void load_json(std::string filename);
  std::chrono::system_clock::time_point convert_datetime(std::string datetime, std::string format);

  int Temperature() { return temperature_; };
  std::chrono::system_clock::time_point CaptureTime() { return capture_time_; };
  std::string Filename() { return datafilename_; };

private:
  std::string dataReaderModule_;
  std::string dataFileExtension_;
  std::string metaFileExtension_;

  VDataReader* data_reader_ = nullptr;

  std::string datafilename_ = "";
  int temperature_ = 0;
  std::chrono::system_clock::time_point capture_time_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_LoadMetaDataFile_H */
