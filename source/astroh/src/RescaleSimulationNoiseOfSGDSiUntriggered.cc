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

#include "RescaleSimulationNoiseOfSGDSiUntriggered.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace comptonsoft
{

RescaleSimulationNoiseOfSGDSiUntriggered::RescaleSimulationNoiseOfSGDSiUntriggered()
  : m_Scale(1.0), m_HitCollection(nullptr)
{
}  

ANLStatus RescaleSimulationNoiseOfSGDSiUntriggered::mod_startup()
{
  register_parameter(&m_Scale, "factor");

  return AS_OK;
}

ANLStatus RescaleSimulationNoiseOfSGDSiUntriggered::mod_init()
{
  GetANLModuleNC("CSHitCollection", &m_HitCollection);

  return AS_OK;
}

ANLStatus RescaleSimulationNoiseOfSGDSiUntriggered::mod_ana()
{
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr& hit: hits) {
      if (hit->isFlags(flag::LowZHit)) { // is Si-hit?
        if (!hit->SelfTriggered()) {
          const double deltaE = hit->EPI() - hit->EnergyCharge();
          const double newEPI = hit->EnergyCharge() + m_Scale * deltaE;
          hit->setEPI(newEPI);
        }
      }
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
