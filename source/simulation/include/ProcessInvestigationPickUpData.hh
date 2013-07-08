/*************************************************************************
 *                                                                       *
 * Copyright (c) 2013 Yuto Ichinohe                                      *
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

#ifndef COMPTONSOFT_ProcessInvestigationPickUpData_H
#define COMPTONSOFT_ProcessInvestigationPickUpData_H

#include "StandardPickUpData.hh"
#include "ConstructDetector_Sim.hh"
#include "ProcessInvestigationTrackingAction.hh"
#include "G4TrackVector.hh"

#include <vector>
#include <map>
#include <string>

class G4Event;
class G4Track;
class G4Step;
class G4Run;

class G4RunManager;
class G4UserStackingAction;

using namespace std;
using namespace anl;
using namespace anlgeant4;

namespace comptonsoft {

/**
 * PickUpData module for process investigation
 * @date 2013-07-08
 * @author Yuto Ichinohe
 */

class ProcessInvestigationPickUpData : public StandardPickUpData {
  DEFINE_ANL_MODULE(ProcessInvestigationPickUpData, 1.2);

public:
  ProcessInvestigationPickUpData();
  void EventAct_begin(const G4Event* aEvent);

  void StepAct(const G4Step* aStep, G4Track* aTrack);
  void CreateUserActions();

  void EventAct_end(const G4Event* aEvent);

  ANLStatus mod_init();
  ANLStatus mod_ana();

  bool fluorescencep() { return m_fluorescencep; }
  bool bremsp() { return m_bremsp; }

  vector<pair<int, pair<int, string> > > getVec() { return vec; }

private:
  ProcessInvestigationTrackingAction* processinvestigationtrackingaction;

  vector<pair<int, pair<int, string> > > vec;

  map<int, int> map_d2m;
  map<int, string> map_d2p;

  map<int, map<string, int> > map_m2d;

  ConstructDetector_Sim* cds;

  void checkFluBrem();
  bool m_fluorescencep;
  bool m_bremsp;

  bool m_ev_rayl;
  int m_hitctr;
  bool m_escapep;
  int n_ev_rayl;

};

}

#endif
