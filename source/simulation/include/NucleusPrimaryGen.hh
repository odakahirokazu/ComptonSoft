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

#ifndef COMPTONSOFT_NucleusPrimaryGen_H
#define COMPTONSOFT_NucleusPrimaryGen_H 1

#include "BasicPrimaryGen.hh"
#include "G4ThreeVector.hh"

class G4ParticleDefinition;

namespace comptonsoft {


/**
 * ANLGeant4 PrimaryGen module.
 * A nucleus is generated.
 *
 * @author Hirokazu Odaka
 * @date 2011-04-12
 */
class NucleusPrimaryGen : public anlgeant4::BasicPrimaryGen
{
  DEFINE_ANL_MODULE(NucleusPrimaryGen, 1.0);
public:
  NucleusPrimaryGen();

  virtual anl::ANLStatus mod_startup();
  virtual anl::ANLStatus mod_bgnrun();
  
private:
  G4ThreeVector m_Position0;
  G4int m_RIZ;
  G4int m_RIA;
  G4double m_RIEnergy;
};

}

#endif /* COMPTONSOFT_NucleusPrimaryGen_H */
