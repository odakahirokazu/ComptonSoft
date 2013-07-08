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

#ifndef ProcessInvestigationTrackingAction_H
#define ProcessInvestigationTrackingAction_H

#include "G4UserTrackingAction.hh"
#include "DetectorManager.hh"
#include "VPickUpData.hh"
#include "HXISGDSensitiveDetector.hh"

#include <vector>
#include <map>
#include <string>
#include <iostream>

using namespace std;
using namespace anlgeant4;

namespace comptonsoft {

class ProcessInvestigationTrackingAction : public G4UserTrackingAction {
/**
 * TrackingAction class for physics process investigation
 * @author Yuto Ichinohe
 * @date 2013-07-08
 */
public:
  ProcessInvestigationTrackingAction();
  ProcessInvestigationTrackingAction(VPickUpData* vpud);
  ~ProcessInvestigationTrackingAction();

  void PreUserTrackingAction(const G4Track* aTrack);
  void PostUserTrackingAction(const G4Track* aTrack);

  vector<pair<int, pair<int, string> > > getVec() { return vec; }
  void clearVec() { vec.clear(); }
  bool escapep() { return m_escapep; }
  void initescapep() { m_escapep = false; }

  void setDetectorManager(DetectorManager* man) { detector_manager = man; }

private:
  VPickUpData* vpickupdata;
  vector<pair<int, pair<int, string> > > vec;
  DetectorManager* detector_manager;
  bool m_escapep;
};

}
#endif
