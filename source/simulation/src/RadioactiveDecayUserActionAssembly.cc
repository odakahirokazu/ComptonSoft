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

#include "RadioactiveDecayUserActionAssembly.hh"
#include "AstroUnits.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

RadioactiveDecayUserActionAssembly::RadioactiveDecayUserActionAssembly()
  : terminationTime_(1.0*unit::ms),
    firstDecayTime_(0.0),
    radioactiveDecayProcessName_("Radioactivation")
{
}

ANLStatus RadioactiveDecayUserActionAssembly::mod_define()
{
  define_parameter("termination_time", &mod_class::terminationTime_, unit::s, "s");
  define_parameter("radioative_decay_process_name", &mod_class::radioactiveDecayProcessName_);

  return AS_OK;
}

void RadioactiveDecayUserActionAssembly::SteppingAction(const G4Step* aStep)
{
  G4Track* aTrack = aStep->GetTrack();
  const double globalTime = aTrack->GetGlobalTime();
  
  if (aTrack->GetTrackID()==1 && aTrack->GetCurrentStepNumber()==1) {
    const G4String processName
      = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    if (processName == radioactiveDecayProcessName_) {
      firstDecayTime_ = globalTime;
      setInitialTime(firstDecayTime_);
    }
    else {
      throw ANLException("RadioactiveDecayUserActionAssembly:Error---First step is not radioactive decay.");
    }
  }

  const double timeFromFirstDecay = globalTime - firstDecayTime_;
  if (timeFromFirstDecay > terminationTime_) {
    aTrack->SetTrackStatus(fKillTrackAndSecondaries);
    return;
  }
}

} /* namespace comptonsoft */
