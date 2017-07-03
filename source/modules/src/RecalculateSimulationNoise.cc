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

#include "RecalculateSimulationNoise.hh"
#include "TRandom3.h"
#include "DetectorHit.hh"
#include "DeviceSimulation.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace comptonsoft
{

RecalculateSimulationNoise::RecalculateSimulationNoise()
  : m_HitCollection(nullptr)
{
}  

ANLStatus RecalculateSimulationNoise::mod_init()
{
  GetModuleNC("CSHitCollection", &m_HitCollection);

  return AS_OK;
}

ANLStatus RecalculateSimulationNoise::mod_ana()
{
  DetectorSystem* detectorManager = getDetectorManager();

  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr& hit: hits) {
      const int detectorID = hit->DetectorID();
      const PixelID pixel = hit->Pixel();
      const double pha = hit->PHA();
      DeviceSimulation* ds = detectorManager->getDeviceSimulationByID(detectorID);
      const double epi = ds->calculateEPI(pha, pixel);
      hit->setEPI(epi);
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
