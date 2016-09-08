/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Shin Watanabe, Hirokazu Odaka                      *
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

// ref file is
// $Id: G4ANLRunManager.cc,v 1.2 2004/03/02 14:30:04 watanabe Exp $
// GEANT4 tag $Name:  $

// ref file is
// $Id: G4ANLRunManager.cc,v 1.2 2004/03/02 14:30:04 watanabe Exp $
// GEANT4 tag $Name:  $

#include "ANLG4RunManager.hh"

namespace anlgeant4
{

void ANLG4RunManager::ANLbgnrunfunc()
{ 
  // line 122
  cond = ConfirmBeamOnCondition();
  if(cond)
  {
    RunInitialization();

    // line 200 in DoEventLoop
    ANLRunstatManager = G4StateManager::GetStateManager();
    if( verboseLevel > 0 )
    {
      timer->Start();
    }
    i_event = 0;
  }
}

void ANLG4RunManager::ANLendrunfunc()
{
  // line 226
  if(cond)
  {
    if( verboseLevel > 0 )
    {
      timer->Stop();
      G4cout << "Run terminated." << G4endl;
      G4cout << "Run Summary" << G4endl;
      if(runAborted)
      {
        G4cout << "  Run Aborted after " << i_event 
               << " events processed." << G4endl;
      }
      else
      {
        G4cout << " Number of events processed : " 
               << i_event << G4endl;
      }
      G4cout << "  " << *timer << G4endl;
    }
    // line 128
    RunTermination();
  }
}

void ANLG4RunManager::ANLanafunc()
{
  // line 217
  currentEvent = GenerateEvent(i_event);
  eventManager->ProcessOneEvent(currentEvent);
  AnalyzeEvent(currentEvent);
  StackPreviousEvent(currentEvent);
  currentEvent = 0;
  i_event++;
}

} /* namespace anlgeant4 */
