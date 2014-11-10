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

#include "MakeDetectorHit.hh"

#include <iostream>

#include "VDeviceSimulation.hh"
// #include "SimDetectorUnit2DStrip.hh"
#include "DetectorUnitDef.hh"

using namespace comptonsoft;

void MakeDetectorHit::doProcessing()
{
  std::vector<VDeviceSimulation*>& dsvec
    = GetDetectorManager()->getDeviceSimulationVector();
  
  std::vector<VDeviceSimulation*>::iterator itDS;
  std::vector<VDeviceSimulation*>::iterator itDSEnd = dsvec.end();

  for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
    (*itDS)->makeDetectorHits();
    (*itDS)->clearRawHits();
  }

  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    (*itDet)->assignDetectorInfo();
    (*itDet)->analyzeHits();
  }
}


bool MakeDetectorHit::setAnalysisParam()
{
  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    std::string prefix = (*itDet)->getNamePrefix();
    if (m_AnalysisMap.count(prefix) == 0) {
      std::cout << "No detector name prefix is found in Threshold Map. " << (*itDet)->getName() << std::endl;
      return false;
    }

    // int type = m_AnalysisMap[prefix].get<0>();
    int mode = m_AnalysisMap[prefix].get<1>();
    (*itDet)->setAnalysisMode(mode);

    double threshold = m_AnalysisMap[prefix].get<2>();
    double thresholdCathode = m_AnalysisMap[prefix].get<3>();
    double thresholdAnode = m_AnalysisMap[prefix].get<4>();

    int detid = (*itDet)->getID();
    VDeviceSimulation* ds = GetDetectorManager()->getDeviceSimulationByID(detid);

    if ((*itDet)->Type().find("SimDetector2DStrip") == 0) {
      ds->setThresholdEnergyCathode(thresholdCathode);
      ds->setThresholdEnergyAnode(thresholdAnode);
    }
    else {
      ds->setThresholdEnergy(threshold);
    }
  }
  return true;
}
