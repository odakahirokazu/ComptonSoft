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

#ifndef COMPTONSOFT_SimXPrimaryGenerator_H
#define COMPTONSOFT_SimXPrimaryGenerator_H 1

#include "BasicPrimaryGenerator.hh"
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
class SimXPrimaryGenerator : public anlgeant4::BasicPrimaryGenerator
{
  DEFINE_ANL_MODULE(SimXPrimaryGenerator, 4.1);
public:
  SimXPrimaryGenerator();
  ~SimXPrimaryGenerator();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;

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

#endif /* COMPTONSOFT_SimXPrimaryGenerator_H */
