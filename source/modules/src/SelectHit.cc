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

#include "SelectHit.hh"

#include <iostream>
#include "globals.hh"
#include "OneASICData.hh"
#include "HXISGDFlagDef.hh"

#include "NextCLI.hh"

using namespace comptonsoft;
using namespace anl;

SelectHit::SelectHit()
  : m_DetectorType(1), m_AnalysisMode(0),
    m_Threshold(1.0*keV), m_ThresholdCathode(1.0*keV), m_ThresholdAnode(1.0*keV)
{
}


ANLStatus SelectHit::mod_startup()
{
  hit_collection = 0;

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
ANLStatus SelectHit::mod_com()
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
#endif


ANLStatus SelectHit::mod_init()
{
  VCSModule::mod_init();

  if (!setAnalysisParam()) {
    return AS_QUIT;
  }

  GetANLModuleNC("CSHitCollection", &hit_collection);

  return AS_OK;
}


ANLStatus SelectHit::mod_ana()
{
  doProcessing();
  collectHits();

  return AS_OK;
}


bool SelectHit::setAnalysisParam()
{
  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    std::string prefix = (*itDet)->getNamePrefix();
    if (m_AnalysisMap.count(prefix) == 0) {
      std::cout << "No detector name prefix is found in Threshold Map. "
		<< (*itDet)->getName() << std::endl;
      return false;
    }

    // int type = m_AnalysisMap[prefix].get<0>();
    int mode = m_AnalysisMap[prefix].get<1>();
    (*itDet)->setAnalysisMode(mode);

    double threshold = m_AnalysisMap[prefix].get<2>();
    double thresholdCathode = m_AnalysisMap[prefix].get<3>();
    double thresholdAnode = m_AnalysisMap[prefix].get<4>();

    if ((*itDet)->Type().find("Detector2DPad") == 0) {
      std::vector<OneASICData*>::iterator itChip;
      std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
      for (itChip = (*itDet)->ASICDataBegin(); itChip != itChipEnd; itChip++) {
        (*itChip)->setThresholdEnergy(threshold);
      }
    }
    else if ((*itDet)->Type().find("Detector2DStrip") == 0) {
      std::vector<OneASICData*>::iterator itChip;
      std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
      for (itChip = (*itDet)->ASICDataBegin(); itChip != itChipEnd; itChip++) {
        if ((*itChip)->isAnodeSide()) {
          (*itChip)->setThresholdEnergy(thresholdAnode);
        }
        else {
          (*itChip)->setThresholdEnergy(thresholdCathode);
        }
      }
    }
    else if ((*itDet)->Type().find("DetectorScintillator") == 0) {
      std::vector<OneASICData*>::iterator itChip;
      std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
      for (itChip = (*itDet)->ASICDataBegin(); itChip != itChipEnd; itChip++) {
        (*itChip)->setThresholdEnergy(threshold);
      }
    }
  }
  return true;
}


void SelectHit::doProcessing()
{
  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    (*itDet)->selectHits();
    (*itDet)->analyzeHits();
  }
}


void SelectHit::collectHits()
{
  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    const int nHit = (*itDet)->NumAnalyzedHits();
    for (int i=0; i<nHit; i++) {
      DetectorHit_sptr hit = (*itDet)->getAnalyzedHit(i);
      InsertHitIntoTheCollection(hit);
    }
  }
}
