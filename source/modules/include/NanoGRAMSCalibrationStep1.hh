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

#ifndef COMPTONSOFT_NanoGRAMSCalibrationStep1_H
#define COMPTONSOFT_NanoGRAMSCalibrationStep1_H 1

#include <cstdint>
#include <memory>
#include <string>

#include "VCSModule.hh"

class TFile;
class TTree;

namespace comptonsoft
{

namespace ngUtil
{
struct StreamingFecHit;
}

class HitTreeIOWithInitialInfo;
class NanoGRAMSDataReduction;

class NanoGRAMSCalibrationStep1 : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSCalibrationStep1, 1.0);

public:
  NanoGRAMSCalibrationStep1();
  ~NanoGRAMSCalibrationStep1() override;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  class Impl;
  std::string config_file_;
  std::string rawhitdata_file_;
  std::string hittree_file_;

  const NanoGRAMSDataReduction* data_reduction_ = nullptr;
  std::unique_ptr<TFile> output_file_;
  TTree* hittree_ = nullptr;
  std::unique_ptr<HitTreeIOWithInitialInfo> tree_io_;
  std::unique_ptr<Impl> impl_;
  std::int64_t written_events_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSCalibrationStep1_H */
