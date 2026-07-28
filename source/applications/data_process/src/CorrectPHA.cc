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

#include "CorrectPHA.hh"
#include <memory>
#include <utility>
#include <boost/format.hpp>
#include "TFile.h"
#include "TH1.h"
#include "TSpline.h"
#include "ReadoutModule.hh"
#include "MultiChannelData.hh"
#include "GainFunctionSpline.hh"

using namespace anlnext;

namespace comptonsoft
{

CorrectPHA::CorrectPHA()
  : PHA_randomization_(false),
    pedestal_correction_(false),
    CMN_subtraction_(CMNSubtractionMode::Given),
    gain_correction_(true),
    pedestal_filename_("pedestal.root"),
    CMN_subtraction_integer_(static_cast<int>(CMNSubtractionMode::Given)),
    gain_filename_("gaincurve.root"),
    gain_file_(nullptr)
{
}

CorrectPHA::~CorrectPHA() = default;

ANLStatus CorrectPHA::mod_define()
{
  define_parameter("pha_randomization", &mod_class::PHA_randomization_);
  set_parameter_description("Randomize PHA values if true");

  define_parameter("pedestal_level", &mod_class::pedestal_filename_);
  set_parameter_description("ROOT file of pedestal levels. '0' for disabling the pedestal level correction.");

  define_parameter("CMN_estimation", &mod_class::CMN_subtraction_integer_);
  set_parameter_description("Calculation method of common mode noise values. 0: no correction, 1: use given value in data, 2: calculate by median, 3: calculate by mean.");

  define_parameter("gain_function", &mod_class::gain_filename_);
  set_parameter_description("ROOT file of calibrated energy gain file. '0' for disabling the gain correction.");

  return AS_OK;
}

ANLStatus CorrectPHA::mod_initialize()
{
  VCSModule::mod_initialize();

  std::unique_ptr<TFile> pedestalFile;
  if (pedestal_filename_=="0") {
    pedestal_correction_ = false;
  }
  else {
    pedestal_correction_ = true;
    pedestalFile.reset( new TFile(pedestal_filename_.c_str()) );
  }

  CMN_subtraction_ = static_cast<CMNSubtractionMode>(CMN_subtraction_integer_);

  if (gain_filename_=="0") {
    gain_correction_ = false;
  }
  else if (gain_filename_=="1") {
    gain_correction_ = true;
  }
  else {
    gain_correction_ = true;
    gain_file_.reset( new TFile(gain_filename_.c_str()) );
  }

  DetectorSystem* detectorManager = getDetectorManager();
  const int NumROM = detectorManager->NumberOfReadoutModules();
  for (int i=0; i<NumROM; i++) {
    ReadoutModule* ROM = detectorManager->getReadoutModuleByIndex(i);
    const int ROMID = ROM->ID();
    const int NumSections = ROM->NumberOfSections();
    for (int j=0; j<NumSections; j++) {
      const DetectorBasedChannelID section = ROM->getSection(j);
      MultiChannelData* mcd = detectorManager->getMultiChannelData(section);
      int NumChannels = mcd->NumberOfChannels();
      if (pedestal_correction_) {
        std::string pedestalName = (boost::format("pedestal/pedestal_r%03d_%03d") % ROMID % j).str();
        TH1D* pedestal = static_cast<TH1D*>( pedestalFile->Get(pedestalName.c_str()) );
        for (int k=0; k<NumChannels; k++) {
          mcd->setPedestal(k, pedestal->GetBinContent(k+1));
        }
      }

      if (gain_correction_ && gain_file_.get()) {
        for (int k=0; k<NumChannels; k++) {
          auto gainFunction = std::make_shared<GainFunctionSpline>();
          const std::string gainName = (boost::format("gain_func_r%03d_%03d_%03d") % i % j % k).str();
          const TSpline* spline = static_cast<const TSpline*>( gain_file_->Get(gainName.c_str()) );
          if (spline == nullptr) {
            std::cout << "CorrectPHA: gain function is not found: " << gainName << std::endl;
          }
          else {
            gainFunction->set(spline);
          }

          mcd->setGainFunction(k, gainFunction);
        }
      }
    }
  }

  if (pedestal_correction_) {
    pedestalFile->Close();
  }

  return AS_OK;
}

ANLStatus CorrectPHA::mod_analyze()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    const int NumSections = detector->NumberOfMultiChannelData();
    for (int j=0; j<NumSections; j++) {
      MultiChannelData* mcd = detector->getMultiChannelData(j);
      mcd->copyToPHA();

      if (PHA_randomization_) {
        mcd->randomizePHAValues();
      }

      if (pedestal_correction_) {
        mcd->correctPedestalLevel();
      }

      if (CMN_subtraction_==CMNSubtractionMode::Median) {
        mcd->calculateCommonModeNoiseByMedian();
      }
      else if (CMN_subtraction_==CMNSubtractionMode::Mean) {
        mcd->calculateCommonModeNoiseByMean();
      }

      if (CMN_subtraction_!=CMNSubtractionMode::No) {
        mcd->subtractCommonModeNoise();
      }

      if (gain_correction_) {
        bool ret = mcd->convertPHA2EPI();
        if (ret == false) {
          std::cout << "MakePI: calibration return status : false" << std::endl;
          return AS_QUIT;
        }
      }
    }
  }

  return AS_OK;
}

ANLStatus CorrectPHA::mod_finalize()
{
  if (gain_file_.get()) {
    gain_file_->Close();
  }
  return AS_OK;
}

} /* namespace comptonsoft */
