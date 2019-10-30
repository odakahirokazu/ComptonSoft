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

  return AS_OK;
}

ANLStatus MakeImageFiles::mod_end_run()
{
  const int num_modules = moduleList_.size();
  canvas_ = new TCanvas("c1", "c1",604, 628);
  for (int i=0; i<num_modules; i++) {
    modules_[i]->drawOutputFiles(canvas_);
  }

  return AS_OK;
}


} /* namespace comptonsoft */
