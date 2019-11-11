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

#ifndef COMPTONSOFT_GetInputFiles_H
#define COMPTONSOFT_GetInputFiles_H 1

#include <anlnext/BasicModule.hh>

namespace comptonsoft {

class LoadFrame;

/**
 * GetInputFiles
 *
 * @author Hirokazu Odaka
 * @date 2019-05-23
 */
class GetInputFiles : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(GetInputFiles, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  GetInputFiles();
  
protected:
  GetInputFiles(const GetInputFiles&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  std::string directory_;
  std::string extension_;
  int delay_ = 0;
  int wait_ = 0;
  LoadFrame* data_reader_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_GetInputFiles_H */
