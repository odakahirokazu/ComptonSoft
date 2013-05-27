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

#ifndef COMPTONSOFT_IsotropicPrimaryGen_H
#define COMPTONSOFT_IsotropicPrimaryGen_H 1

#include "BasicPrimaryGen.hh"
#include "G4ThreeVector.hh"

namespace comptonsoft {


/**
 * ANLGeant4 PrimaryGen module.
 * Isotropic and homogeneous particle distribution are realized.
 *
 * @author Hirokazu Odaka
 * @date 2010-xx-xx
 */
class IsotropicPrimaryGen : public anlgeant4::BasicPrimaryGen
{
  DEFINE_ANL_MODULE(IsotropicPrimaryGen, 1.1);
public:
  IsotropicPrimaryGen();
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_prepare();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_endrun();

protected:
  double Radius() const { return m_Radius; }
  double CoveringFactor() const { return m_CoveringFactor; }
  
private:
  G4ThreeVector m_CenterPosition;
  G4double m_Radius;
  G4ThreeVector m_CenterDirection;
  G4double m_ThetaMin;
  G4double m_ThetaMax;
  G4double m_CosTheta0;
  G4double m_CosTheta1;
  G4double m_CoveringFactor;

  G4double m_Flux; // energy per unit {time, area, solid angle}
};

}

#endif /* COMPTONSOFT_IsotropicPrimaryGen_H */
