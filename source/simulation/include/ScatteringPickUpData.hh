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

#include "VAppendableUserActionAssembly.hh"

class TTree;

namespace comptonsoft {

/**
 * PickUpData for first scattering
 *
 * @author Hirokazu Odaka
 * @date 2008-08-27
 * @date 2011-04-08
 * @date 2017-06-29 | redesign of VAppendableUserActionAssembly
 */
class ScatteringPickUpData : public anlgeant4::VAppendableUserActionAssembly
{
  DEFINE_ANL_MODULE(ScatteringPickUpData, 3.0);
public:
  ScatteringPickUpData();
  
  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_init() override;

  void EventActionAtBeginning(const G4Event*) override;
  void SteppingAction(const G4Step* aStep) override;

private:
  std::string processName_;

  bool firstInteraction_ = false;
  
  TTree* tree_ = nullptr;
  
  double dirx_ = 0.0;
  double diry_ = 0.0;
  double dirz_ = 0.0;
  double energy_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ScatteringPickUpData_H */
