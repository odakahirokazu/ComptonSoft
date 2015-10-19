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

#include "MakeRawHits.hh"

#include <iostream>
#include "DeviceSimulation.hh"

namespace comptonsoft
{

anl::ANLStatus MakeRawHits::mod_startup()
{
  SelectHits::mod_startup();
  hide_parameter("analysis_map");
  return anl::AS_OK;
}

void MakeRawHits::doProcessing()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto ds: detectorManager->getDeviceSimulationVector()) {
    ds->makeRawDetectorHits();
  }

  for (auto& detector: detectorManager->getDetectors()) {
    detector->assignReadoutInfo();
    detector->reconstructHits();
  }
}

bool MakeRawHits::setAnalysisParam()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    detector->setReconstructionMode(0);
  }
  for (auto ds: detectorManager->getDeviceSimulationVector()) {
    ds->resetThresholdVector(0.0);
  }
  return true;
}

} /* namespace comptonsoft */
