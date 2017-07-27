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
  : m_Filename("output.root"),
    m_RootFile(nullptr)
{
}

SaveData::~SaveData() = default;

ANLStatus SaveData::mod_define()
{
  register_parameter(&m_Filename, "output");
  return AS_OK;
}

ANLStatus SaveData::mod_pre_initialize()
{
  m_RootFile.reset(new TFile(m_Filename.c_str(), "recreate"));
  if ( !m_RootFile ) {
    std::cout << "SaveData: cannot create ROOT file" << std::endl;
    return AS_QUIT;
  }
  
  return AS_OK;
}

ANLStatus SaveData::mod_finalize()
{
  std::cout << "SaveData: saving data to ROOT file" << std::endl;
  m_RootFile->Write();
  std::cout << "SaveData: closing ROOT file" << std::endl;
  m_RootFile->Close();
  std::cout << "SaveData: ROOT file closed " << std::endl;
  
  return AS_OK;
}

TDirectory* SaveData::GetDirectory()
{
  return m_RootFile->GetDirectory(0);
}

bool SaveData::cd()
{
  return m_RootFile->cd();
}

} /* namespace comptonsoft */
