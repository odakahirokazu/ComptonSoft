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

#include "AssignTimeWithExpDistribution.hh"

#include <random>
#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

AssignTimeWithExpDistribution::AssignTimeWithExpDistribution()
{
}

AssignTimeWithExpDistribution::~AssignTimeWithExpDistribution() = default;

ANLStatus AssignTimeWithExpDistribution::mod_define()
{
  define_parameter("time_start", &mod_class::m_Time, unit::s, "s");
  define_parameter("count_rate", &mod_class::m_CountRate, 1.0/unit::s, "s^-1");
  define_parameter("random_seed", &mod_class::m_Seed);
  set_parameter_description("Random seed, unsigned integer, -1 for hardware generation.");
  return AS_OK;
}

ANLStatus AssignTimeWithExpDistribution::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &m_HitCollection);
  std::random_device seedGenerator;
  const uint_fast32_t seed = (m_Seed == -1) ? seedGenerator() : m_Seed;
  m_Engine.seed(m_Seed);
  m_Distribution = std::exponential_distribution<double>(m_CountRate);
  return AS_OK;
}

ANLStatus AssignTimeWithExpDistribution::mod_analyze()
{
  const double dt = m_Distribution(m_Engine);
  m_Time += dt;
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr hit: hits) {
      const double realTime = m_Time + hit->RealTime();
      hit->setRealTime(realTime);
      const double time = m_Time + hit->Time();
      hit->setTime(time);
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
