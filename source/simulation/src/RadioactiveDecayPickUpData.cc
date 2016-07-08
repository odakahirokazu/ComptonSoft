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

#include "RadioactiveDecayPickUpData.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

using namespace anl;

namespace comptonsoft
{

RadioactiveDecayPickUpData::RadioactiveDecayPickUpData()
  : m_TerminationTime(1.0*ms), m_FirstDecayTime(0.0)
{
  SetStepActOn(true);
}

ANLStatus RadioactiveDecayPickUpData::mod_startup()
{
  register_parameter(&m_TerminationTime, "termination_time", s, "s");

  return AS_OK;
}

void RadioactiveDecayPickUpData::StepAct(const G4Step* aStep, G4Track* aTrack)
{
  const double globalTime = aTrack->GetGlobalTime();
  
  if (aTrack->GetTrackID()==1 && aTrack->GetCurrentStepNumber()==1) {
    const G4String processName
      = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    if (processName == "RadioactiveDecay") {
      m_FirstDecayTime = globalTime;
      setInitialTime(m_FirstDecayTime);
    }
    else {
      throw ANLException("RadioactiveDecayPickUpData:Error---First step is not radioactive decay.");
    }
  }

  const double timeFromFirstDecay = globalTime - m_FirstDecayTime;
  if (timeFromFirstDecay > m_TerminationTime) {
    aTrack->SetTrackStatus(fStopAndKill);
    return;
  }
}

} /* namespace comptonsoft */
