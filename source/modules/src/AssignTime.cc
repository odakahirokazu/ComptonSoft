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
  set_parameter_description("-1 denotes all.");
  define_parameter("time_start", &mod_class::m_Time0, unit::s, "s");
  define_parameter("count_rate", &mod_class::m_CountRate, 1.0/unit::s, "s^-1");
  define_parameter("exposure", &mod_class::m_Exposure, unit::s, "s");
  define_parameter("random_seed", &mod_class::m_Seed);
  set_parameter_description("Random seed, unsigned integer, -1 for hardware generation.");
  define_parameter("sort", &mod_class::m_SortTime);
  set_parameter_description("If true, the time array is sorted.");
  return AS_OK;
}

ANLStatus AssignTime::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &m_HitCollection);
  get_module_NC("ReadEventTree", &m_ReadEventTree);

  if (m_NumEvents == -1) {
    m_NumEvents = m_ReadEventTree->NumEntries();
  }

  if (m_CountRate>0 && m_Exposure>0) {
    std::cout << "ERROR: Both count_rate and exposure are set. Should delete either of the two." << std::endl;
    return AS_QUIT_ERROR;
  }

  if (m_CountRate>0) {
    m_Time1 = m_Time0 + m_NumEvents/m_CountRate;
  }
  else if (m_Exposure>0) {
    m_Time1 = m_Time0 + m_Exposure;
  }
  else {
    std::cout << "ERROR: Either of count_rate or exposure should be set positive." << std::endl;
    return AS_QUIT_ERROR;
  }
  
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
  if (m_SortTime) {
    m_TimeList.sort();
  }
}

} /* namespace comptonsoft */
