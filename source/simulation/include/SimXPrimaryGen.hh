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

#ifndef COMPTONSOFT_SimXPrimaryGen_H
#define COMPTONSOFT_SimXPrimaryGen_H 1

#include "BasicPrimaryGen.hh"
#include "G4ThreeVector.hh"
#include "PhaseSpaceVector.hh"

namespace comptonsoft {

class SimXIF;


/**
 * SimX Interface module to Geant4 Primary generator
 * @author Hirokazu Odaka
 * @date 2012-02-16
 * @date 2012-06-29
 * @date 2017-07-27 | makePrimarySetting()
 */
class SimXPrimaryGen : public anlgeant4::BasicPrimaryGen
{
  DEFINE_ANL_MODULE(SimXPrimaryGen, 4.1);
public:
  SimXPrimaryGen();
  ~SimXPrimaryGen();
 
  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_endrun() override;

  void makePrimarySetting() override;
  
private:
  SimXIF* m_SimXIF;

  G4ThreeVector m_CenterPosition;
  G4double m_Radius;

  G4ThreeVector m_Polarization0;
  G4double m_PolarizationDegree;

  G4double m_Flux; // energy per unit {time, area}
};

}

#endif /* COMPTONSOFT_SimXPrimaryGen_H */
