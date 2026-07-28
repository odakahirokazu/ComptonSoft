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

#include "SaveData.hh"
#include <cstdio>
#include <iostream>
#include "TFile.h"

using namespace anlnext;

namespace comptonsoft
{

SaveData::SaveData()
  : filename_("output.root"),
    root_file_(nullptr)
{
}

SaveData::~SaveData() = default;

ANLStatus SaveData::mod_define()
{
  define_parameter("output", &mod_class::filename_);
  define_parameter("period", &mod_class::period_);
  return AS_OK;
}

ANLStatus SaveData::mod_pre_initialize()
{
  root_file_.reset(new TFile(filename_.c_str(), "recreate"));
  if ( !root_file_ ) {
    std::cout << "SaveData: cannot create ROOT file" << std::endl;
    return AS_QUIT;
  }

  return AS_OK;
}

ANLStatus SaveData::mod_analyze()
{
  const int period = period_;
  if (period==0) { return AS_OK; }

  const int loop_count = get_loop_index()+1;
  if (loop_count%period == 0) {
    root_file_->Write();
  }
  return AS_OK;
}

ANLStatus SaveData::mod_finalize()
{
  std::cout << "SaveData: saving data to ROOT file" << std::endl;
  root_file_->Write();
  std::cout << "SaveData: closing ROOT file" << std::endl;
  root_file_->Close();
  std::cout << "SaveData: ROOT file closed " << std::endl;

  return AS_OK;
}

TDirectory* SaveData::GetDirectory()
{
  return root_file_->GetDirectory(0);
}

bool SaveData::cd()
{
  return root_file_->cd();
}

} /* namespace comptonsoft */
