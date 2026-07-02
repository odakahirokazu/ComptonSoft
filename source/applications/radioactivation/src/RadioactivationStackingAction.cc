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

#include "RadioactivationStackingAction.hh"

#include "G4Track.hh"
#include "G4ParticleTypes.hh"

namespace comptonsoft
{

RadioactivationStackingAction::RadioactivationStackingAction() = default;

RadioactivationStackingAction::~RadioactivationStackingAction() = default;

G4ClassificationOfNewTrack RadioactivationStackingAction::ClassifyNewTrack(const G4Track* track)
{
  /* selection: fKill, fUrgent, fSuspend */
  G4ClassificationOfNewTrack classification = fUrgent;

  /* kill if the particle is not relevant to radioactivation */
  if (track->GetParentID() != 0) {
    G4ParticleDefinition* particleType = track->GetDefinition();
    if ((particleType == G4Gamma::GammaDefinition())
        || (particleType == G4Electron::ElectronDefinition())
        || (particleType == G4Positron::PositronDefinition())
        || (particleType == G4NeutrinoE::NeutrinoEDefinition())
        || (particleType == G4AntiNeutrinoE::AntiNeutrinoEDefinition())) {

      classification = fKill;
    }
  }

  return classification;
}

} /* namespace comptonsoft */
