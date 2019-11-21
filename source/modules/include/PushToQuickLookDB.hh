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

/**
 * PushToQuickLookDB
 *
 * @author Tsubasa Tamba, Hirokazu Odaka
 * @date 2019-11-05
 */

#ifndef COMPTONSOFT_PushToQuickLookDB_H
#define COMPTONSOFT_PushToQuickLookDB_H 1

#include <anlnext/BasicModule.hh>
#include "VCSModule.hh"
#include "TCanvas.h"

class TH2;

namespace hsquicklook {
class MongoDBClient;
}

namespace comptonsoft {

class PushToQuickLookDB : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(PushToQuickLookDB, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  PushToQuickLookDB();
  
protected:
  PushToQuickLookDB(const PushToQuickLookDB&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

protected:
  void pushImagesToDB();

private:
  int canvas_width_ = 1;
  int canvas_height_ = 1;
  std::vector<std::string> moduleList_;
  std::string collection_;
  std::string directory_;
  std::string document_;
  int period_ = 1;
  int phase_ = 0;
  std::vector<VCSModule*> modules_;
  TCanvas* canvas_;
  std::vector<std::string> fileList_;
  hsquicklook::MongoDBClient* mongodb_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PushToQuickLookDB_H */
