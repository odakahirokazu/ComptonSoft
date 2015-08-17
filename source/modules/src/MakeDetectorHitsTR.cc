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

#include "MakeDetectorHitsTR.hh"

#include <iostream>
#include <algorithm>
#include "DeviceSimulation.hh"
#include "FlagDefinition.hh"

namespace comptonsoft
{

void MakeDetectorHitsTR::doProcessing()
{
  DetectorSystem* detectorManager = getDetectorManager();
  auto dsVector = detectorManager->getDeviceSimulationVector();
  for (auto ds: dsVector) {
    ds->initializeTimingProcess();
  }
 
  int maxTimeGroup = 0;
  for (int timeGroup = 0; ; timeGroup++) {
    m_TriggerTimes.clear();
    
    for (auto ds: dsVector) {
      if (!(ds->isEmptySimulatedHits())) {
        m_TriggerTimes.push_back(ds->FirstEventTime());
      }
    }

    if (m_TriggerTimes.empty()) break;

    const double triggerTime = *std::min_element(m_TriggerTimes.begin(),
                                                 m_TriggerTimes.end());
    
    for (auto ds: dsVector) {
      ds->makeDetectorHitsAtTime(triggerTime, timeGroup);
    }

    maxTimeGroup = timeGroup;
  }

  for (auto& detector: detectorManager->getDetectors()) {
    detector->assignReadoutInfo();
    detector->reconstructHits();
  }
}

} /* namespace comptonsoft */
