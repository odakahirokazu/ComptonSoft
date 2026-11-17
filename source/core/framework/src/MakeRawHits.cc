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

anlnext::ANLStatus MakeRawHits::mod_define()
{
  SelectHits::mod_define();
  hide_parameter("analysis_map");
  return anlnext::AS_OK;
}

anlnext::ANLStatus MakeRawHits::mod_initialize()
{
  const auto status = SelectHits::mod_initialize();
  if (status != anlnext::AS_OK) { return status; }
  // Raw deposits have no measured pixel yet. Reconstruction would replace
  // their local position with the centre of an undefined pixel.
  configure_raw_hit_recording();
  return anlnext::AS_OK;
}

void MakeRawHits::doProcessing()
{
  DetectorSystem* detectorManager = getDetectorManager();
  detectorManager->distributeRawHitsToDetectors();

  for (auto ds: detectorManager->getDeviceSimulationVector()) {
    ds->makeRawDetectorHits();
  }

  for (auto& detector: detectorManager->getDetectors()) {
    detector->reconstructHits();
  }
}

void MakeRawHits::configure_raw_hit_recording()
{
  DetectorSystem* detectorManager = getDetectorManager();
  for (auto& detector: detectorManager->getDetectors()) {
    detector->setReconstructionMode(0);
  }
  for (auto ds: detectorManager->getDeviceSimulationVector()) {
    ds->resetThresholdVector(0.0);
  }
}

} /* namespace comptonsoft */
