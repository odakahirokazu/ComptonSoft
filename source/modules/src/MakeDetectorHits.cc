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

#include "MakeDetectorHits.hh"
#include <iostream>
#include "DeviceSimulation.hh"
#include "SimDetectorUnit2DStrip.hh"

namespace comptonsoft
{

void MakeDetectorHits::doProcessing()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto ds: detectorManager->getDeviceSimulationVector()) {
    ds->makeDetectorHits();
  }

  for (auto& detector: detectorManager->getDetectors()) {
    detector->assignReadoutInfo();
    detector->reconstructHits();
  }
}

bool MakeDetectorHits::setThresholdEnergy(VRealDetectorUnit* detector,
                                          double threshold,
                                          double thresholdCathode,
                                          double thresholdAnode)
{
  const int detid = detector->getID();
  if (detector->checkType(DetectorType::DoubleSidedStripDetector)) {
    SimDetectorUnit2DStrip* ds
      = dynamic_cast<SimDetectorUnit2DStrip*>(detector);
    if (ds == nullptr) {
      std::cout << "The detector object can not be converted into SimDetector2DStrip. Detector ID: " << detid << std::endl;
      return false;
    }

    auto xStripSelector = std::mem_fn(&PixelID::isXStrip);
    auto yStripSelector = std::mem_fn(&PixelID::isYStrip);
    if (ds->isXStripSideCathode()) {
      // cathode
      ds->setThresholdToSelected(thresholdCathode, xStripSelector);
      // anode
      ds->setThresholdToSelected(thresholdAnode, yStripSelector);
    }
    else if (ds->isXStripSideAnode()){
      // anode
      ds->setThresholdToSelected(thresholdAnode, xStripSelector);
      // cathode
      ds->setThresholdToSelected(thresholdCathode, yStripSelector);
    }
  }
  else {
    VDeviceSimulation* ds
      = dynamic_cast<VDeviceSimulation*>(detector);
    if (ds == nullptr) {
      std::cout << "The detector object can not be converted into VDeviceSimulation. Detector ID: " << detid << std::endl;
      return false;
    }
    ds->resetThresholdVector(threshold);
  }
  return true;
}

} /* namespace comptonsoft */
