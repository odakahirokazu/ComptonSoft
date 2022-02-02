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


#ifndef COMPTONSOFT_PushHistogramToQuickLookDB_H
#define COMPTONSOFT_PushHistogramToQuickLookDB_H 1

#include <anlnext/BasicModule.hh>
#include "VCSModule.hh"
#include "XrayEventCollection.hh"
#include "LoadMetaDataFile.hh"

namespace hsquicklook {
class MongoDBClient;
}

namespace comptonsoft {

class PushHistogramToQuickLookDB : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(PushHistogramToQuickLookDB, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  PushHistogramToQuickLookDB();
  
protected:
  PushHistogramToQuickLookDB(const PushHistogramToQuickLookDB&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  void pushRecentHistograms();
  void pushProfile();
  void initializeHistograms();
  void loadHistogramsFromMongoDB();
  void updateHistograms();
  void updateTrends();
  void updateMFparameters();

private:
  std::string event_collection_module_name_;
  std::string recent_collection_name_;
  std::string profile_collection_name_;
  std::string directory_;
  std::string analysis_id_ = "";

  std::vector<std::string> columns_name_;
  std::vector<double> columns_min_;
  std::vector<double> columns_max_;
  std::vector<int> columns_bin_;

  std::vector<std::vector<int>> histograms_;

  // vectors
  std::vector<int> temperature_trend_;
  std::vector<int> count_trend_;

  // for modulation factor
  int Nh_ = 0, Nv_ = 0;
  std::string mf_output_ = "";

  const double epi = 1e-4;

  const XrayEventCollection* event_collection_module_ = nullptr;
  const LoadMetaDataFile* metadata_file_module_ = nullptr;
  hsquicklook::MongoDBClient* mongodb_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PushHistogramToQuickLookDB_H */
