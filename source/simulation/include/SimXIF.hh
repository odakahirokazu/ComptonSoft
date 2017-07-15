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

#ifndef COMPTONSOFT_SimXIF_H
#define COMPTONSOFT_SimXIF_H 1

#include <anl/BasicModule.hh>
#include "PhaseSpaceVector.hh"


namespace simx {

struct PARAMETERS;
struct Source_Type;
struct Response_Type;
struct Event_Type;

}

namespace comptonsoft {

class DetectorSystem;


/**
 * SimX Interface module
 * @author Hirokazu Odaka
 * @date 2012-02-16 | v 1.0 |
 * @date 2012-06-29 | v 1.1 |
 * @date 2012-10-11 | v 1.2 | Detector manager
 * @date 2013-01-21 | v 2.0 | SimX 2.0
 */
class SimXIF : public anl::BasicModule
{
  DEFINE_ANL_MODULE(SimXIF, 2.0);
public:
  SimXIF();
  ~SimXIF();
  
  anl::ANLStatus mod_pre_initialize() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;

  void generatePrimaries(double area);
  PhaseSpaceVector takePrimary();
  void outputPrimaries(const std::string& file_name);
  size_t NumberOfPrimaries() const { return m_Primaries.size(); }

  void addEvent(double time, double energy, int stripx, int stripy,
                int detector_id);
  void outputEvents();

  int findPI(double energy);

private:
  // SimX IF
  struct simx::PARAMETERS* m_SimXParameters;
  struct simx::Source_Type* m_Source;
  struct simx::Response_Type* m_Response;
  struct simx::Event_Type* m_EventList;
  struct simx::Event_Type* m_Event;
  int m_SimXStatus;

  std::vector<PhaseSpaceVector> m_Primaries;
  std::vector<PhaseSpaceVector>::const_iterator m_PrimaryIter;

  comptonsoft::DetectorSystem* m_DetectorManager;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimXIF_H */
