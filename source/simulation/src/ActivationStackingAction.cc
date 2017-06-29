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

#include "ActivationStackingAction.hh"

#include "G4Track.hh"
#include "G4ParticleTypes.hh"

namespace comptonsoft
{

ActivationStackingAction::ActivationStackingAction() = default;

ActivationStackingAction::~ActivationStackingAction() = default;

G4ClassificationOfNewTrack 
ActivationStackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  /* selection: fKill, fUrgent, fSuspend */
  G4ClassificationOfNewTrack classification = fUrgent;
  
  /* kill if the particle is not relevant to radioactivation */
  if (aTrack->GetParentID() != 0) {
    G4ParticleDefinition* particleType = aTrack->GetDefinition();
    if ((particleType == G4Gamma::GammaDefinition())
        || (particleType == G4Electron::ElectronDefinition())
        || (particleType == G4Positron::PositronDefinition())
        || (particleType == G4NeutrinoE::NeutrinoEDefinition())
        || (particleType == G4AntiNeutrinoE::AntiNeutrinoEDefinition())) {
      
      classification = fKill;
      // G4cout << "ClassifyNewTrack() -- kill" << G4endl;
    }
  }
  
  return classification;
}

} /* namespace comptonsoft */
