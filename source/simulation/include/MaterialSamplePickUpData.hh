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

#ifndef COMPTONSOFT_MaterialSamplePickUpData_H
#define COMPTONSOFT_MaterialSamplePickUpData_H 1

#include "StandardPickUpData.hh"

class TTree;
class TH1D;
class G4PhotoElectricEffect;

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {


class MaterialSamplePickUpData : public anlgeant4::StandardPickUpData
{
  DEFINE_ANL_MODULE(MaterialSamplePickUpData, 1.0);
public:
  MaterialSamplePickUpData();
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  
  void StepAct(const G4Step* aStep, G4Track* aTrack);
  
private:
  TTree* masstree;
  
protected:
  double ini_energy;

  double ini_dirx;
  double ini_diry;
  double ini_dirz;
  
  double ini_posx;
  double ini_posy;
  double ini_posz;
	
private:
  const anlgeant4::InitialInformation* primary;
  //  std::string filename;
  double mass;
  double tau1;
  double tau2;
  double tau3;
  double tau4;
  double tau5;
  
  G4PhotoElectricEffect* m_PhAbsProcess;
};

}

#endif /* COMPTONSOFT_MaterialSamplePickUpData_H */
