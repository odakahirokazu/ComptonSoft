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

#include "MakeRawHit.hh"

#include <iostream>
#include "VDeviceSimulation.hh"

using namespace comptonsoft;


anl::ANLStatus MakeRawHit::mod_startup()
{
  SelectHit::mod_startup();
  hide_parameter("Analysis map");
  return anl::AS_OK;
}


void MakeRawHit::doProcessing()
{
  std::vector<VDeviceSimulation*>& dsvec
    = GetDetectorManager()->getDeviceSimulationVector();
  
  std::vector<VDeviceSimulation*>::iterator itDS;
  std::vector<VDeviceSimulation*>::iterator itDSEnd = dsvec.end();

  for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
    (*itDS)->makeRawDetectorHits();
    (*itDS)->clearRawHits();
  }

  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    (*itDet)->analyzeHits();
  }
}


bool MakeRawHit::setAnalysisParam()
{
  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    int mode = 0;
    (*itDet)->setAnalysisMode(mode);
    int detid = (*itDet)->getID();
    GetDetectorManager()->getDeviceSimulationByID(detid)
      ->setThresholdEnergy(0.0);
  }
  return true;
}
