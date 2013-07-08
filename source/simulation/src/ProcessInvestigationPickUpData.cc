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

#include "ProcessInvestigationPickUpData.hh"

#include "G4RunManager.hh"
#include "PickUpDataEventAction.hh"
#include "PickUpDataRunAction.hh"
#include "PickUpDataSteppingAction.hh"
#include "ProcessInvestigationTrackingAction.hh"

#include "G4Event.hh"
#include "G4VProcess.hh"

#include "cs_globals.hh"
#include "DetectorManager.hh"
#include "DeviceSimulation.hh"
#include "RealDetectorUnit.hh"
#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include "HXISGDFlagDef.hh"


#include <iostream>

using namespace std;
using namespace anlgeant4;
using namespace comptonsoft;


ProcessInvestigationPickUpData::ProcessInvestigationPickUpData()
  : StandardPickUpData(), processinvestigationtrackingaction(0),
    m_fluorescencep(false), m_bremsp(false),
    m_ev_rayl(false), m_hitctr(0), m_escapep(false), n_ev_rayl(0)
{
  add_alias("ProcessInvestigationPickUpData");
  SetStepActOn(true);
}

ANLStatus ProcessInvestigationPickUpData::mod_init() {
  processinvestigationtrackingaction->clearVec();
  return AS_OK;
}

ANLStatus ProcessInvestigationPickUpData::mod_ana() {
  DetectorManager* dm = cds->GetDetectorManager();

  vector<VDeviceSimulation*> dsvec
    = dm->getDeviceSimulationVector();

  vector<VDeviceSimulation*>::iterator itDS;
  vector<VDeviceSimulation*>::iterator itDSEnd = dsvec.end();

  int numrawctr = 0;
  int numrayl = 0;
  int num0ene = 0;

  if (m_ev_rayl) {
    for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
      for (int i = 0; i < (*itDS)->NumRawHits(); i++) {
        ++numrawctr;

        DetectorHit_sptr tmpdh;
        tmpdh = (*itDS)->getRawHitNC(i);

        tmpdh->addProcess(EV_RAYL);

        if ((tmpdh->getProcess() & PROC_RAYLEIGH) == PROC_RAYLEIGH) {
          ++numrayl;
        }
        if ((tmpdh->getProcess() & PROC_0ENE) == PROC_0ENE) {
          ++num0ene;
        }

        string particletype = tmpdh->getParticleType();
        int trackid = tmpdh->getTrackID();
        unsigned int process = tmpdh->getProcess();

        if (particletype == "gamma") {
          int motherid = map_d2m[trackid];
          if (motherid == 0) {    // incident gamma
            tmpdh->addProcess(ORIG_INCIDENT);
          } else {                // brems or fluorescence
            if (map_d2p[motherid] == "gamma") { // fluorescence
              tmpdh->addProcess(ORIG_FLUOR);
            } else {                                     // brems
              tmpdh->addProcess(ORIG_BREMS);
            }
          }

          if (process&PROC_PHOTOABS) {
            if (map_m2d[trackid].find("gamma") != map_m2d[trackid].end()) { // fluorescence
              tmpdh->addProcess(PROC_FLUOR);
            }
          }
        }
      }
    }
  } else {
    for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
      for (int i = 0; i < (*itDS)->NumRawHits(); i++) {
        ++numrawctr;

        DetectorHit_sptr tmpdh;
        tmpdh = (*itDS)->getRawHitNC(i);

        if ((tmpdh->getProcess() & PROC_0ENE) == PROC_0ENE) {
          ++num0ene;
        }
        string particletype = tmpdh->getParticleType();
        int trackid = tmpdh->getTrackID();
        unsigned int process = tmpdh->getProcess();

        if (particletype == "gamma") {
          int motherid = map_d2m[trackid];
          if (motherid == 0) {    // incident gamma
            tmpdh->addProcess(ORIG_INCIDENT);
          } else {                // brems or fluorescence
            if (map_d2p[motherid] == "gamma") { // fluorescence
              tmpdh->addProcess(ORIG_FLUOR);
            } else {                                     // brems
              tmpdh->addProcess(ORIG_BREMS);
            }
          }

          if (process&PROC_PHOTOABS) {
            if (map_m2d[trackid].find("gamma") != map_m2d[trackid].end()) { // fluorescence
              tmpdh->addProcess(PROC_FLUOR);
            }
          }
        }
      }
    }
  }

  bool tmp_escape = m_escapep;
  bool tmp_passive(false);
  if (m_hitctr + numrayl + num0ene != numrawctr) {
    tmp_passive = true;
  }

  bool tmp_raylonlysd(true);
  if (n_ev_rayl != numrayl) {
    tmp_raylonlysd = false;
  }


  unsigned int tmp_flag = 0x00000000u;

  if (tmp_raylonlysd) {
    tmp_flag |= EV_RAYLONLYSD;
  }

  if (tmp_escape) {
    tmp_flag |= EV_ESCAPE;
  }
  if (tmp_passive) {
    tmp_flag |= EV_PASSIVE;
  }

  for (itDS = dsvec.begin(); itDS != itDSEnd; itDS++) {
    for (int i = 0; i < (*itDS)->NumRawHits(); i++) {
      DetectorHit_sptr tmpdh;
      tmpdh = (*itDS)->getRawHitNC(i);
      tmpdh->addProcess(tmp_flag);
    }
  }

  return AS_OK;
}

void ProcessInvestigationPickUpData::EventAct_begin(const G4Event* aEvent) {
  StandardPickUpData::EventAct_begin(aEvent);
  m_fluorescencep = false;
  m_bremsp = false;
  m_ev_rayl = false;
  vec.clear();
  map_d2m.clear();
  map_d2p.clear();
  map_m2d.clear();
  processinvestigationtrackingaction->clearVec();
  processinvestigationtrackingaction->initescapep();
  m_hitctr = 0;
  m_escapep = false;
  n_ev_rayl = 0;
}

void ProcessInvestigationPickUpData::EventAct_end(const G4Event*) {
  vec = processinvestigationtrackingaction->getVec();
  vector<pair<int, pair<int, string> > >::iterator vec_it = vec.begin();
  while (vec_it != vec.end()) {
    map_d2m.insert(make_pair((*vec_it).second.first, (*vec_it).first));
    map_d2p.insert(make_pair((*vec_it).second.first, (*vec_it).second.second));
    map_m2d[(*vec_it).first].insert(make_pair((*vec_it).second.second, (*vec_it).second.first));
    ++vec_it;
  }
  m_escapep = processinvestigationtrackingaction->escapep();
}

void ProcessInvestigationPickUpData::StepAct(const G4Step* aStep, G4Track*) {
  const G4VProcess* proc = aStep->GetPostStepPoint()->GetProcessDefinedStep();
  G4String procname = proc->GetProcessName();
  if (procname.find("Rayl") != std::string::npos) {
    m_ev_rayl = true;
    ++n_ev_rayl;
  }
  if (aStep->GetTotalEnergyDeposit() > 1.0e-15) { // e- coupled transportation (edep e-26 toka wo mushi)
    ++m_hitctr;
  }
}
void ProcessInvestigationPickUpData::CreateUserActions() {
  GetANLModuleNC("ConstructDetector_Sim", &cds);

  processinvestigationtrackingaction = new ProcessInvestigationTrackingAction(this);
  SetTrackingAction(processinvestigationtrackingaction);
}

void ProcessInvestigationPickUpData::checkFluBrem() {
  map<int, string> gammavec;

  vector<pair<int, pair<int, string> > >::iterator vec_it = vec.begin();
  ++vec_it;
  gammavec.insert(make_pair(1, ""));
  while (vec_it != vec.end()) {
    if ((*vec_it).second.second == "gamma") {
      gammavec.insert(make_pair((*vec_it).second.first, ""));
      if (gammavec.find((*vec_it).first) != gammavec.end()) {
        m_fluorescencep = true;
      } else {
        m_bremsp = true;
      }
    }
    ++vec_it;
  }
}
