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

#include "SelectHits.hh"

#include <iostream>
#include "AstroUnits.hh"
#include "MultiChannelData.hh"
#include "FlagDefinition.hh"
#include "RealDetectorUnit2DStrip.hh"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

SelectHits::SelectHits()
  : m_DetectorType(1), m_ReconstructionMode(0),
    m_Threshold(1.0*unit::keV), m_ThresholdCathode(1.0*unit::keV), m_ThresholdAnode(1.0*unit::keV),
    m_LowerECheckFuncC0(0.0), m_LowerECheckFuncC1(0.9),
    m_UpperECheckFuncC0(0.0), m_UpperECheckFuncC1(1.1)
{
}

SelectHits::~SelectHits() = default;

ANLStatus SelectHits::mod_startup()
{
  register_parameter_map(&m_AnalysisMap, "analysis_map",
                         "detector_name_prefix", "Si");
  add_value_element(&m_DetectorType, "detector_type", "",
                    "Detector type (1: pad, 2: DSD, 3: scintillator)");
  add_value_element(&m_ReconstructionMode, "reconstruction_mode");
  add_value_element(&m_Threshold, "threshold", unit::keV, "keV");
  add_value_element(&m_ThresholdCathode, "threshold_cathode", unit::keV, "keV");
  add_value_element(&m_ThresholdAnode, "threshold_anode", unit::keV, "keV");
  add_value_element(&m_LowerECheckFuncC0, "lower_energy_consistency_check_function_c0", unit::keV, "keV");
  add_value_element(&m_LowerECheckFuncC1, "lower_energy_consistency_check_function_c1");
  add_value_element(&m_UpperECheckFuncC0, "upper_energy_consistency_check_function_c0", unit::keV, "keV");
  add_value_element(&m_UpperECheckFuncC1, "upper_energy_consistency_check_function_c1");
  enable_value_elements(1, {1, 2});
  enable_value_elements(2, {1, 3, 4, 5, 6, 7, 8});
  enable_value_elements(3, {1, 2});
  
  return AS_OK;
}

ANLStatus SelectHits::mod_init()
{
  VCSModule::mod_init();
  GetModuleNC("CSHitCollection", &m_HitCollection);
  return AS_OK;
}

ANLStatus SelectHits::mod_ana()
{
  doProcessing();
  collectHits();
  return AS_OK;
}

bool SelectHits::setAnalysisParameters()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    const std::string prefix = detector->getNamePrefix();
    if (m_AnalysisMap.count(prefix) == 0) {
      std::cout << "No detector name prefix is found in Threshold Map. "
                << detector->getName() << std::endl;
      return false;
    }

    auto analysisParameters = m_AnalysisMap[prefix];
    const int type = std::get<0>(analysisParameters);
    if (!detector->checkType(type)) {
      std::cout << "Detector type given in the analysis parameters is inconsistent.\n"
                << "  prefix: " << prefix << "\n"
                << "  type: " << type << " => should be " << static_cast<int>(detector->Type())
                << std::endl;
      return false;
    }
    
    const int mode = std::get<1>(analysisParameters);
    detector->setReconstructionMode(mode);
    
    const double threshold = std::get<2>(analysisParameters);
    const double thresholdCathode = std::get<3>(analysisParameters);
    const double thresholdAnode = std::get<4>(analysisParameters);
    if (!setThresholdEnergy(detector.get(),
                            threshold, thresholdCathode, thresholdAnode)) {
      std::cout << "SelectHits: setThresholdEnergy() failed." << std::endl;
      return false;
    }

    if (detector->checkType(DetectorType::DoubleSidedStripDetector)) {
      // coefficients of energy consistency check functions
      const double lowerFuncC0 = std::get<5>(analysisParameters);
      const double lowerFuncC1 = std::get<6>(analysisParameters);
      const double upperFuncC0 = std::get<7>(analysisParameters);
      const double upperFuncC1 = std::get<8>(analysisParameters);
      setEnergyConsistencyCheckFunctions(detector.get(),
                                         lowerFuncC0, lowerFuncC1,
                                         upperFuncC0, upperFuncC1);
    }
  }

  return true;
}

bool SelectHits::setThresholdEnergy(VRealDetectorUnit* detector,
                                    double threshold,
                                    double thresholdCathode,
                                    double thresholdAnode)
{
  if (detector->checkType(DetectorType::DoubleSidedStripDetector)) {
    const int NSections = detector->NumberOfMultiChannelData();
    for (int section=0; section<NSections; section++) {
      MultiChannelData* mcd = detector->getMultiChannelData(section);
      if (mcd->isAnodeSide()) {
        mcd->resetThresholdEnergyVector(thresholdAnode);
      }
      else if (mcd->isCathodeSide()) {
        mcd->resetThresholdEnergyVector(thresholdCathode);
      }
    }
  }
  else {
    const int NSections = detector->NumberOfMultiChannelData();
    for (int section=0; section<NSections; section++) {
      MultiChannelData* mcd = detector->getMultiChannelData(section);
      mcd->resetThresholdEnergyVector(threshold);
    }
  }
  return true;
}

bool SelectHits::setEnergyConsistencyCheckFunctions(VRealDetectorUnit* detector,
                                                    double lowerC0,
                                                    double lowerC1,
                                                    double upperC0,
                                                    double upperC1)
{
  if (detector->checkType(DetectorType::DoubleSidedStripDetector)) {
    RealDetectorUnit2DStrip* dsd = dynamic_cast<RealDetectorUnit2DStrip*>(detector);
    if (dsd) {
      if (dsd->isEnergyConsistencyRequired()) {
        dsd->setEnergyConsistencyCheckFunctions(lowerC0, lowerC1, upperC0, upperC1);
      }
    }
  }
  return true;
}

void SelectHits::doProcessing()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    detector->selectHits();
    detector->reconstructHits();
  }
}

void SelectHits::collectHits()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    const int numHits = detector->NumberOfReconstructedHits();
    for (int i=0; i<numHits; i++) {
      DetectorHit_sptr hit = detector->getReconstructedHit(i);
      this->insertHitIntoTheCollection(hit);
    }
  }
}

} /* namespace comptonsoft */
