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

#include "MakeDetectorHitTR.hh"

#include <iostream>
#include <algorithm>
#include "VDeviceSimulation.hh"
#include "HXISGDFlagDef.hh"

using namespace comptonsoft;

void MakeDetectorHitTR::doProcessing()
{
  std::vector<VDeviceSimulation*>& dsvec
    = GetDetectorManager()->getDeviceSimulationVector();
  
  std::vector<VDeviceSimulation*>::iterator itDS;
  std::vector<VDeviceSimulation*>::iterator itDSEnd = dsvec.end();

  for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
    (*itDS)->initializeTimingProcess();
    (*itDS)->clearRawHits();
  }
 
  int maxTimeGroup = comptonsoft::NOTIMEGROUP;
  for (int timeGroup = 0; ; timeGroup++) {
    trigger_times.clear();

    for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
      if (!(*itDS)->EmptySimulatedHits()) {
	trigger_times.push_back((*itDS)->FirstEventTime());
      }
    }

    if (trigger_times.empty()) break;

    const double triggerTime = *std::min_element(trigger_times.begin(),
						 trigger_times.end());

    for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
      (*itDS)->makeDetectorHitsAtTime(triggerTime, timeGroup);
    }

    maxTimeGroup = timeGroup;
  }

  std::vector<RealDetectorUnit*>::iterator itDet;
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  for (itDet = GetDetectorVector().begin(); itDet != itDetEnd; itDet++) {
    (*itDet)->analyzeHits();
  }
}
