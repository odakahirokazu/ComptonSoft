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

#include "CreateRootFile.hh"
#include <boost/format.hpp>
#include "TFile.h"
#include "TMemFile.h"

using namespace anlnext;

namespace comptonsoft
{

CreateRootFile::CreateRootFile()
  : filename_base_("output"),
    root_file_(nullptr)
{
}

CreateRootFile::~CreateRootFile() = default;

CreateRootFile::CreateRootFile(const CreateRootFile& r)
  : BasicModule::BasicModule(r),
    filename_base_(r.filename_base_),
    root_file_(nullptr),
    separate_clones_(r.separate_clones_),
    save_clones_(r.save_clones_)
{
}

std::string CreateRootFile::Filename() const
{
  if (is_master() && master_file_) { return FilenameBase()+".root"; }
  return FilenameBase()+(boost::format("_%03d.root")%copy_id()).str();
}

ANLStatus CreateRootFile::mod_define()
{
  define_parameter("filename_base", &mod_class::filename_base_);
  define_parameter("master_file", &mod_class::master_file_);
  define_parameter("parallel", &mod_class::separate_clones_);
  define_parameter("save_parallel", &mod_class::save_clones_);
  return AS_OK;
}

ANLStatus CreateRootFile::mod_initialize()
{
  if (is_master()) {
    root_file_.reset(new TFile(Filename().c_str(), "recreate"));
  }
  else if (separate_clones_) {
    if (save_clones_) {
      root_file_.reset(new TFile(Filename().c_str(), "recreate"));
    }
    else {
      root_file_.reset(new TMemFile(Filename().c_str(), "recreate"));
    }
  }
  else {
    return AS_OK;
  }

  if ( !root_file_ ) {
    std::cout << "CreateRootFile: cannot create ROOT file" << std::endl;
    return AS_QUIT;
  }

  return AS_OK;
}

ANLStatus CreateRootFile::mod_finalize()
{
  if (is_master() || (separate_clones_ && save_clones_)) {
    std::cout << "CreateRootFile: saving data to ROOT file " << Filename() << std::endl;
    root_file_->Write();
  }

  if (is_master() || separate_clones_) {
    root_file_->Close();
    std::cout << "CreateRootFile: ROOT file closed " << std::endl;
  }

  return AS_OK;
}

TDirectory* CreateRootFile::GetDirectory()
{
  return root_file_->GetDirectory(0);
}

bool CreateRootFile::cd()
{
  return root_file_->cd();
}

} /* namespace comptonsoft */
