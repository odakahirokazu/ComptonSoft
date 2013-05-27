/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011                                                    *
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

#ifndef COMPTONSOFT_KillStepPickUpData_H
#define COMPTONSOFT_KillStepPickUpData_H 1

#include "StandardPickUpData.hh"

#include <string>


class G4ParticleDefinition;

namespace comptonsoft {


class KillStepPickUpData : public anlgeant4::StandardPickUpData
{
  DEFINE_ANL_MODULE(KillStepPickUpData, 1.0);
public:
  KillStepPickUpData();
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  
  void StepAct(const G4Step* aStep, G4Track* aTrack);
  
private:
  std::string m_ParticleName;
  std::string stopping_volume_name;
  //int enable;
  
  G4ParticleDefinition* m_ParticleType;
	
};

}

#endif /* COMPTONSOFT_KillStepPickUpData_H */
