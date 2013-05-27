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

// 2008-04-30 Hirokazu Odaka
// 2008-04-30 Hirokazu Odaka : ANLLite

#ifndef COMPTONSOFT_SaveData_H
#define COMPTONSOFT_SaveData_H 1

#include "BasicModule.hh"
#include "TFile.h"


namespace comptonsoft {

class SaveData : public anl::BasicModule
{
  DEFINE_ANL_MODULE(SaveData, 2.0);
public:
  SaveData();
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_exit();

  TDirectory* GetDirectory() { return m_RootFile->GetDirectory(0); }
  bool cd() { return m_RootFile->cd(); }
  std::string FileName() const { return m_FileName; }
private:
  std::string m_FileName;
  TFile* m_RootFile;
};

}

#endif /* COMPTONSOFT_SaveData_H */
