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

#include <anlnext/BasicModule.hh>
#include <memory>

class TDirectory;
class TFile;

namespace comptonsoft {

/**
 * Module to manage TFile for save histograms/trees.
 * @author Hirokazu Odaka
 * @date 2017-07-07 | based on SaveData
 */
class CreateRootFile : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(CreateRootFile, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  CreateRootFile();
  ~CreateRootFile();

protected:
  CreateRootFile(const CreateRootFile& r);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;

  TDirectory* GetDirectory();
  bool cd();

  std::string FilenameBase() const { return filename_base_; }
  std::string Filename() const;

private:
  std::string filename_base_;
  std::unique_ptr<TFile> root_file_;
  bool master_file_ = true;
  bool separate_clones_ = false;
  bool save_clones_ = false;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CreateRootFile_H */
