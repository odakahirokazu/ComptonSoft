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

#include "NextCLI.hh"

using namespace anl;

namespace comptonsoft
{

SelectHits::SelectHits()
  : m_DetectorType(1), m_AnalysisMode(0),
    m_Threshold(1.0*keV), m_ThresholdCathode(1.0*keV), m_ThresholdAnode(1.0*keV)
{
}

SelectHits::~SelectHits() = default;

ANLStatus SelectHits::mod_startup()
{
  register_parameter_map(&m_AnalysisMap, "analysis_map",
                         "detector_name_prefix", "Si");
  add_map_value(&m_DetectorType, "detector_type", "",
                "Detector type (1: pad, 2: DSD, 3: scintillator)");
  add_map_value(&m_AnalysisMode, "analysis_mode");
  add_map_value(&m_Threshold, "threshold", keV, "keV");
  add_map_value(&m_ThresholdCathode, "threshold_cathode", keV, "keV");
  add_map_value(&m_ThresholdAnode, "threshold_anode", keV, "keV");
  std::vector<int> enable1(2); enable1[0] = 1; enable1[1]= 2;
  std::vector<int> enable2(3); enable2[0] = 1; enable2[1]= 3; enable2[2] = 4;
  std::vector<int> enable3(2); enable3[0] = 1; enable3[1]= 2;
  enable_map_value(1, enable1);
  enable_map_value(2, enable2);
  enable_map_value(3, enable3);
  
  return AS_OK;
}

#if 0
ANLStatus SelectHits::mod_com()
{
  std::string name("DSSD");
  std::cout << "Definition of analysis methods : " << std::endl;

  while (1) {
    CLread("Detector name prefix (OK for exit)", &name);
    if (name == "OK" or name == "ok") { break; }

    int type = 1;
    CLread("Detector type (1:pad, 2:double-sided strip, 3:scintillator)", &type);
    
    if (type == 1) {
      int analysis_mode = 0;
      CLread("Analysis Mode (0:simple, 1:normal, 2:clustering)", &analysis_mode);
      double threshold = 10.0*keV;
      CLread("Threshold energy", &threshold, keV, "keV");
      m_AnalysisMap.insert(std::make_pair(name, boost::make_tuple(analysis_mode, threshold, 0.0)));
    }
    else if (type == 2) {
      int analysis_mode = 0;
      CLread("Analysis Mode (0:simple, 1:max-height, 2:single-hit, 3:clustering+max-height, 4:clustering+single-hit", &analysis_mode);
      double threshold1 = 10.0*keV;
      CLread("Threshold energy (anode)", &threshold1, keV, "keV");
      double threshold2 = threshold1;
      CLread("Threshold energy (cathode)", &threshold2, keV, "keV");
      m_AnalysisMap.insert(std::make_pair(name, boost::make_tuple(analysis_mode, threshold1, threshold2)));
    }
    else if (type == 3) {
      int analysis_mode = 0;
      CLread("Analysis Mode (0:simple, 1:normal)", &analysis_mode);
      double threshold = 10.0*keV;
      CLread("Threshold energy", &threshold, keV, "keV");
      m_AnalysisMap.insert(std::make_pair(name, boost::make_tuple(analysis_mode, threshold, 0.0)));
    }
    
    name = "OK";
  }

  return AS_OK;
}
#endif /* 0 */

ANLStatus SelectHits::mod_init()
{
  VCSModule::mod_init();

  if (!setAnalysisParam()) {
    return AS_QUIT;
  }

  GetANLModuleNC("CSHitCollection", &m_HitCollection);

  return AS_OK;
}

ANLStatus SelectHits::mod_ana()
{
  doProcessing();
  collectHits();

  return AS_OK;
}

bool SelectHits::setAnalysisParam()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    std::string prefix = detector->getNamePrefix();
    if (m_AnalysisMap.count(prefix) == 0) {
      std::cout << "No detector name prefix is found in Threshold Map. "
                << detector->getName() << std::endl;
      return false;
    }

    // int type = m_AnalysisMap[prefix].get<0>();
    int mode = m_AnalysisMap[prefix].get<1>();
    detector->setAnalysisMode(mode);

    double threshold = m_AnalysisMap[prefix].get<2>();
    double thresholdCathode = m_AnalysisMap[prefix].get<3>();
    double thresholdAnode = m_AnalysisMap[prefix].get<4>();
    bool rval = setThresholdEnergy(detector.get(),
                                   threshold, thresholdCathode, thresholdAnode);
    if (!rval) return rval;
  }
  return true;
}

bool SelectHits::setThresholdEnergy(VRealDetectorUnit* detector,
                                   double threshold,
                                   double thresholdCathode,
                                   double thresholdAnode)
{
  if (detector->Type().find("Detector2DStrip") == 0) {
    const int NSections = detector->NumberOfMultiChannelData();
    for (int section=0; section<NSections; section++) {
      MultiChannelData* mcd = detector->getMultiChannelData(section);
      if (mcd->isAnodeSide()) {
        mcd->setThresholdEnergy(thresholdAnode);
      }
      else if (mcd->isCathodeSide()) {
        mcd->setThresholdEnergy(thresholdCathode);
      }
    }
  }
  else {
    const int NSections = detector->NumberOfMultiChannelData();
    for (int section=0; section<NSections; section++) {
      MultiChannelData* mcd = detector->getMultiChannelData(section);
      mcd->setThresholdEnergy(threshold);
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
