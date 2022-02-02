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

#ifndef COMPTONSOFT_SaveData_H
#define COMPTONSOFT_SaveData_H 1

#include <anlnext/BasicModule.hh>
#include <memory>

class TFile;

namespace comptonsoft {

/**
 * Module to manage TFile for save histograms/trees.
 * @author Hirokazu Odaka
 * @date 2008-04-30
 * @date 2017-03-23 | use unique_ptr for the root file.
 */
class SaveData : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SaveData, 3.0);
public:
  SaveData();
  ~SaveData();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_finalize() override;

  TDirectory* GetDirectory();
  bool cd();

  std::string Filename() const { return m_Filename; }

private:
  std::string m_Filename;
  std::unique_ptr<TFile> m_RootFile;
  int m_Period = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SaveData_H */
