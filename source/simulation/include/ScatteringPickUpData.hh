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

#ifndef COMPTONSOFT_ScatteringPickUpData_H
#define COMPTONSOFT_ScatteringPickUpData_H 1

#include "StandardPickUpData.hh"
#include "TTree.h"

namespace comptonsoft {


/**
 * PickUpData for radioactive decay.
 *
 * @author Hirokazu Odaka
 * @date 2008-08-27
 * @date 2011-04-08
 */
class ScatteringPickUpData : public anlgeant4::StandardPickUpData
{
  DEFINE_ANL_MODULE(ScatteringPickUpData, 2.0);
public:
  ScatteringPickUpData();
  
  virtual anl::ANLStatus mod_startup();
  virtual anl::ANLStatus mod_his();

  virtual void EventAct_begin(const G4Event*);
  virtual void StepAct(const G4Step* aStep, G4Track* aTrack);

private:
  TTree* m_Tree;
  double m_DirX;
  double m_DirY;
  double m_DirZ;
  double m_Energy;
  bool m_FirstInteraction;
  std::string m_ProcessName;
};

}

#endif /* COMPTONSOFT_ScatteringPickUpData_H */
