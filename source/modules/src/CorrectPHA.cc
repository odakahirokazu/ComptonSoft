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

using namespace anl;

namespace comptonsoft
{

CorrectPHA::CorrectPHA()
  : m_PHARandomization(false),
    m_PedestalCorrection(false),
    m_CMNSubtraction(CMNSubtractionMode::Given),
    m_GainCorrection(true),
    m_PedestalFileName("pedestal.root"),
    m_CMNSubtractionInteger(static_cast<int>(CMNSubtractionMode::Given)),
    m_GainFileName("gaincurve.root"),
    m_GainFile(nullptr)
{
}

CorrectPHA::~CorrectPHA() = default;

ANLStatus CorrectPHA::mod_define()
{
  register_parameter(&m_PHARandomization, "pha_randomization");
  set_parameter_description("Randomize PHA values if true");
  
  register_parameter(&m_PedestalFileName, "pedestal_level");
  set_parameter_description("ROOT file of pedestal levels. '0' for disabling the pedestal level correction.");

  register_parameter(&m_CMNSubtractionInteger, "CMN_estimation");
  set_parameter_description("Calculation method of common mode noise values. 0: no correction, 1: use given value in data, 2: calculate by median, 3: calculate by mean.");
  
  register_parameter(&m_GainFileName, "gain_function");
  set_parameter_description("ROOT file of calibrated energy gain file. '0' for disabling the gain correction.");
  
  return AS_OK;
}

ANLStatus CorrectPHA::mod_initialize()
{
  VCSModule::mod_initialize();

  std::unique_ptr<TFile> pedestalFile;
  if (m_PedestalFileName=="0") {
    m_PedestalCorrection = false;
  }
  else {
    m_PedestalCorrection = true;
    pedestalFile.reset( new TFile(m_PedestalFileName.c_str()) );
  }

  m_CMNSubtraction = static_cast<CMNSubtractionMode>(m_CMNSubtractionInteger);

  if (m_GainFileName=="0") {
    m_GainCorrection = false;
  }
  else if (m_GainFileName=="1") {
    m_GainCorrection = true;
  }
  else {
    m_GainCorrection = true;
    m_GainFile.reset( new TFile(m_GainFileName.c_str()) );
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
      if (m_PedestalCorrection) {
        std::string pedestalName = (boost::format("pedestal/pedestal_r%03d_%03d") % ROMID % j).str();
        TH1D* pedestal = static_cast<TH1D*>( pedestalFile->Get(pedestalName.c_str()) );
        for (int k=0; k<NumChannels; k++) {
          mcd->setPedestal(k, pedestal->GetBinContent(k+1));
        }
      }

      if (m_GainCorrection && m_GainFile.get()) {
        std::unique_ptr<GainFunctionSpline> gainFunction(new GainFunctionSpline(NumChannels));
        for (int k=0; k<NumChannels; k++) {
          std::string gainName = (boost::format("gain_func_r%03d_%03d_%03d") % i % j % k).str();
          const TSpline* spline = static_cast<const TSpline*>( m_GainFile->Get(gainName.c_str()) );
          if (spline == nullptr) {
            std::cout << "CorrectPHA: gain function is not found: " << gainName << std::endl;
          }
          else {
            gainFunction->set(k, spline);
          }
        }
        mcd->registerGainFunction(std::move(gainFunction));
      }
    }
  }

  if (m_PedestalCorrection) {
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

      if (m_PHARandomization) {
        mcd->randomizePHAValues();
      }

      if (m_PedestalCorrection) {
        mcd->correctPedestalLevel();
      }
      
      if (m_CMNSubtraction==CMNSubtractionMode::Median) {
        mcd->calculateCommonModeNoiseByMedian();
      }
      else if (m_CMNSubtraction==CMNSubtractionMode::Mean) {
        mcd->calculateCommonModeNoiseByMean();
      }

      if (m_CMNSubtraction!=CMNSubtractionMode::No) {
        mcd->subtractCommonModeNoise();
      }

      if (m_GainCorrection) {
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
  if (m_GainFile.get()) {
    m_GainFile->Close();
  }
  return AS_OK;
}

} /* namespace comptonsoft */
