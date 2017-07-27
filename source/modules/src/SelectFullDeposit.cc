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

#include "SelectFullDeposit.hh"
#include <cmath>
#include "DetectorHit.hh"
#include "InitialInformation.hh"
#include "CSHitCollection.hh"

using namespace anlnext;

namespace comptonsoft
{

ANLStatus SelectFullDeposit::mod_initialize()
{
  get_module_NC("CSHitCollection", &m_HitCollection);
  get_module_IF("InitialInformation", &m_InitialInfo);
  
  return AS_OK;
}

ANLStatus SelectFullDeposit::mod_analyze()
{
  const std::vector<DetectorHit_sptr>& hits
    = m_HitCollection->getHits();

  double energy = 0.0;
  for (auto& hit: hits) {
    energy += hit->Energy();
  }

  const double FullDepositThreshold = 0.03;
  const double initialEnergy = m_InitialInfo->InitialEnergy();

  if (std::abs(energy/initialEnergy - 1.0) > FullDepositThreshold) {
    return AS_SKIP;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
