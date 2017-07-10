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

using namespace anl;

namespace comptonsoft
{

CreateRootFile::CreateRootFile()
  : m_FilenameBase("output"),
    m_RootFile(nullptr)
{
}

CreateRootFile::~CreateRootFile() = default;

CreateRootFile::CreateRootFile(const CreateRootFile& r)
  : BasicModule::BasicModule(r),
    m_FilenameBase(r.m_FilenameBase),
    m_RootFile(nullptr),
    m_SeparateClones(r.m_SeparateClones),
    m_SaveClones(r.m_SaveClones)
{
}

std::string CreateRootFile::Filename() const
{
  if (is_master() && m_MasterFile) { return FilenameBase()+".root"; }
  return FilenameBase()+(boost::format("_%03d.root")%copy_id()).str();
}

ANLStatus CreateRootFile::mod_define()
{
  register_parameter(&m_FilenameBase, "filename_base");
  register_parameter(&m_MasterFile, "master_file");
  register_parameter(&m_SeparateClones, "parallel");
  register_parameter(&m_SaveClones, "save_parallel");
  return AS_OK;
}

ANLStatus CreateRootFile::mod_initialize()
{
  if (is_master()) {
    m_RootFile.reset(new TFile(Filename().c_str(), "recreate"));
  }
  else if (m_SeparateClones) {
    if (m_SaveClones) {
      m_RootFile.reset(new TFile(Filename().c_str(), "recreate"));
    }
    else {
      m_RootFile.reset(new TMemFile(Filename().c_str(), "recreate"));
    }
  }
  else {
    return AS_OK;
  }

  if ( !m_RootFile ) {
    std::cout << "CreateRootFile: cannot create ROOT file" << std::endl;
    return AS_QUIT;
  }
  
  return AS_OK;
}

ANLStatus CreateRootFile::mod_finalize()
{
  if (is_master() || (m_SeparateClones && m_SaveClones)) {
    std::cout << "CreateRootFile: saving data to ROOT file " << Filename() << std::endl;
    m_RootFile->Write();
  }

  if (is_master() || m_SeparateClones) {
    m_RootFile->Close();
    std::cout << "CreateRootFile: ROOT file closed " << std::endl;
  }
  
  return AS_OK;
}

TDirectory* CreateRootFile::GetDirectory()
{
  return m_RootFile->GetDirectory(0);
}

bool CreateRootFile::cd()
{
  return m_RootFile->cd();
}

} /* namespace comptonsoft */
