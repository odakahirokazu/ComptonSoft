#include <iostream>

#include "ProcessInvestigationTrackingAction.hh"
#include "cs_globals.hh"
#include "VPickUpData.hh"

#include "G4TrackingManager.hh"
#include "G4Trajectory.hh"
#include "G4VTouchable.hh"

using namespace std;
using namespace anlgeant4;
using namespace comptonsoft;

ProcessInvestigationTrackingAction::ProcessInvestigationTrackingAction() {
  G4cout << "Constructing ProcessInvestigationTrackingAction " << G4endl;
}

ProcessInvestigationTrackingAction::ProcessInvestigationTrackingAction(VPickUpData* vpud) {
  vpickupdata = vpud;
  G4cout << "Constructing ProcessInvestigationTrackingAction " << G4endl;
}

ProcessInvestigationTrackingAction::~ProcessInvestigationTrackingAction() {
  G4cout << "Destructing ProcessInvestigationTrackingAction " << G4endl;
}

void ProcessInvestigationTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(new G4Trajectory(aTrack));

  vpickupdata->TrackAct_begin(aTrack);

  vec.push_back(make_pair(aTrack->GetParentID(),
                          make_pair(aTrack->GetTrackID(),
                                    aTrack->GetDefinition()->GetParticleName())));
}

void ProcessInvestigationTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
  if (aTrack->GetKineticEnergy() > 0.0) {
    m_escapep = true;
  }
  vpickupdata->TrackAct_end(aTrack);
}
