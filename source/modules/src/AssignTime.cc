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

#include "AssignTime.hh"

#include <random>
#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

AssignTime::AssignTime()
{
}

AssignTime::~AssignTime() = default;

ANLStatus AssignTime::mod_define()
{
  define_parameter("number_of_events", &mod_class::m_NumEvents);
  define_parameter("time_start", &mod_class::m_Time0, unit::s, "s");
  define_parameter("time_end", &mod_class::m_Time1, unit::s, "s");
  define_parameter("random_seed", &mod_class::m_Seed);
  set_parameter_description("Random seed, unsigned integer, -1 for hardware generation.");
  return AS_OK;
}

ANLStatus AssignTime::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &m_HitCollection);

  sampleEventTimes();

  return AS_OK;
}

ANLStatus AssignTime::mod_analyze()
{
  if (m_TimeList.empty()) {
    return AS_QUIT;
  }

  const double t = m_TimeList.front();
  m_TimeList.pop_front();
  
  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr hit: hits) {
      const double realTime = t + hit->RealTime();
      hit->setRealTime(realTime);
    }
  }

  return AS_OK;
}

void AssignTime::sampleEventTimes()
{
  std::random_device seedGenerator;
  const uint_fast32_t seed = (m_Seed == -1) ? seedGenerator() : m_Seed;
  std::mt19937 engine(seed);
  std::uniform_real_distribution<double> distribution(m_Time0, m_Time1);

  for (int i=0; i<m_NumEvents; i++) {
    const double t = distribution(engine);
    m_TimeList.push_back(t);
  }

  m_TimeList.sort();
}

} /* namespace comptonsoft */
