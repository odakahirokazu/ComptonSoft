/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#ifndef COMPTONSOFT_SampleOpticalDepth_H
#define COMPTONSOFT_SampleOpticalDepth_H 1

#include "VAppendableUserActionAssembly.hh"

class TTree;
class G4VEmProcess;

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {

/**
 * @author Hirokazu Odaka, Tamotsu Sato
 * @date 2017-07-29 | Hirokazu Odaka | new design of VAppendableUserActionAssembly, code cleanup.
 */
class SampleOpticalDepth : public anlgeant4::VAppendableUserActionAssembly
{
  DEFINE_ANL_MODULE(SampleOpticalDepth, 2.0);
public:
  SampleOpticalDepth();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;

  void EventActionAtBeginning(const G4Event*) override;
  void EventActionAtEnd(const G4Event*) override;
  void SteppingAction(const G4Step* aStep) override;
  
private:
  double energy_;
  std::string processName_;
  std::string particleName_;
  
  const anlgeant4::InitialInformation* initialInfo_ = nullptr;
  G4VEmProcess* process_ = nullptr;

  TTree* tree_ = nullptr;
  
  double ini_posx_ = 0.0;
  double ini_posy_ = 0.0;
  double ini_posz_ = 0.0;
  double ini_dirx_ = 0.0;
  double ini_diry_ = 0.0;
  double ini_dirz_ = 0.0;
  double length_ = 0.0;
  double tau_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SampleOpticalDepth_H */
