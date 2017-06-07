/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka, Makoto Asai                        *
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

#include "RadioactiveDecayStackingAction.hh"

#include "G4Track.hh"
#include "G4ParticleTypes.hh"

using namespace comptonsoft;


RadioactiveDecayStackingAction::RadioactiveDecayStackingAction()
{
  G4cout << "RadioactiveDecayStackingAction: constructed" << G4endl;
}


RadioactiveDecayStackingAction::~RadioactiveDecayStackingAction()
{
  G4cout << "RadioactiveDecayStackingAction: destructed" << G4endl;
}


G4ClassificationOfNewTrack 
RadioactiveDecayStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  /* selection: fKill, fUrgent, fSuspend */
  G4ClassificationOfNewTrack classification = fUrgent;
  
  /* kill if the particle is not relevant to radioactivation */
  if (aTrack->GetParentID() != 0) {
    const double t = aTrack->GetGlobalTime() - firstDecayTime_;
    if (t > terminationTime_) {
      classification = fKill;
    }
  }
  
  return classification;
}
