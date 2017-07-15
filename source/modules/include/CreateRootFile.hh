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

#ifndef COMPTONSOFT_CreateRootFile_H
#define COMPTONSOFT_CreateRootFile_H 1

#include <anl/BasicModule.hh>
#include <memory>

class TDirectory;
class TFile;

namespace comptonsoft {

/**
 * Module to manage TFile for save histograms/trees.
 * @author Hirokazu Odaka
 * @date 2017-07-07 | based on SaveData
 */
class CreateRootFile : public anl::BasicModule
{
  DEFINE_ANL_MODULE(CreateRootFile, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  CreateRootFile();
  ~CreateRootFile();
  
protected:
  CreateRootFile(const CreateRootFile& r);

public:
  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_finalize() override;

  TDirectory* GetDirectory();
  bool cd();

  std::string FilenameBase() const { return m_FilenameBase; }
  std::string Filename() const;

private:
  std::string m_FilenameBase;
  std::unique_ptr<TFile> m_RootFile;
  bool m_MasterFile = true;
  bool m_SeparateClones = false;
  bool m_SaveClones = false;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CreateRootFile_H */
