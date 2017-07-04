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

#include "UniformlyRandomizeEPI.hh"
#include "TRandom3.h"
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

UniformlyRandomizeEPI::UniformlyRandomizeEPI()
  : m_Random(new TRandom3)
{
}  

UniformlyRandomizeEPI::~UniformlyRandomizeEPI() = default;

ANLStatus UniformlyRandomizeEPI::mod_startup()
{
  register_parameter(&m_LZDeltaE0, "si_delta0", unit::keV, "keV");
  register_parameter(&m_LZDeltaE1, "si_delta1", unit::keV, "keV");
  register_parameter(&m_HZDeltaE0, "cdte_delta0", unit::keV, "keV");
  register_parameter(&m_HZDeltaE1, "cdte_delta1", unit::keV, "keV");

  return AS_OK;
}

ANLStatus UniformlyRandomizeEPI::mod_init()
{
  GetModuleNC("CSHitCollection", &m_HitCollection);

  return AS_OK;
}

ANLStatus UniformlyRandomizeEPI::mod_ana()
{
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr& hit: hits) {
      if (hit->isFlags(flag::LowZHit)) { // is Si-hit?
        const double deltaE = m_Random->Uniform(m_LZDeltaE0, m_LZDeltaE1);
        const double newEPI = hit->EPI() + deltaE;
        hit->setEPI(newEPI);
      }
      else if (hit->isFlags(flag::HighZHit)) { // is CdTe-hit?
        const double deltaE = m_Random->Uniform(m_HZDeltaE0, m_HZDeltaE1);
        const double newEPI = hit->EPI() + deltaE;
        hit->setEPI(newEPI);
      }
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
