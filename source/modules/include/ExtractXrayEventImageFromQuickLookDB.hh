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


#ifndef COMPTONSOFT_ExtractXrayEventImageFromQuickLookDB_H
#define COMPTONSOFT_ExtractXrayEventImageFromQuickLookDB_H 1

#include "ExtractXrayEventImage.hh"

namespace comptonsoft {

class ExtractXrayEventImageFromQuickLookDB : public ExtractXrayEventImage
{
  DEFINE_ANL_MODULE(ExtractXrayEventImageFromQuickLookDB, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  ExtractXrayEventImageFromQuickLookDB();
  
protected:
  ExtractXrayEventImageFromQuickLookDB(const ExtractXrayEventImageFromQuickLookDB&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  void setLatestAnalysisId();

private:
  std::string collectionName_;
  int collectionType_ = 0;
  int initial_frame_id_;
  int max_frame_per_loop_;
  std::string HostName_;
  std::string dbName_;
  std::string analysisId_;

  int last_frame_id_;
  XrayEventCollection* collection_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ExtractXrayEventImageFromQuickLookDB_H */
