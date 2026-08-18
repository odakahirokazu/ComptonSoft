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

#ifndef COMPTONSOFT_NanoGRAMSWriteHitTree_H
#define COMPTONSOFT_NanoGRAMSWriteHitTree_H 1

#include <cstdint>
#include <memory>
#include <string>

#include "NanoGRAMSCalibrationData.hh"
#include "VCSModule.hh"

class TFile;
class TTree;

namespace comptonsoft
{

class HitTreeIOWithInitialInfo;
class NanoGRAMSReadTPCEvents;

class NanoGRAMSWriteHitTree : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSWriteHitTree, 1.0);

public:
  NanoGRAMSWriteHitTree();
  ~NanoGRAMSWriteHitTree() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  void flushOutput();

  std::string hittree_file_;
  int output_flush_entries_ = 1000;

  const NanoGRAMSReadTPCEvents* tpc_events_ = nullptr;
  std::unique_ptr<TFile> output_file_;
  TTree* hit_tree_ = nullptr;
  std::unique_ptr<HitTreeIOWithInitialInfo> tree_io_;
  CalibrationConfig calibration_config_;
  int64_t written_events_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSWriteHitTree_H */
