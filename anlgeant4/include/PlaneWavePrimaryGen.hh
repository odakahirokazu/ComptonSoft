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

#ifndef ANLGEANT4_PlaneWavePrimaryGen_H
#define ANLGEANT4_PlaneWavePrimaryGen_H 1

#include "BasicPrimaryGen.hh"
#include "G4ThreeVector.hh"

namespace anlgeant4 {


/**
 * ANLGeant4 PrimaryGen module.
 * The primary particles are generated like a plane wave.
 *
 * @author Hirokazu Odaka
 * @date 2010-02-17
 * @date 2010-04-08 | Hirokazu Odaka | ANLLite
 * @date 2011-04-08 | Hirokazu Odaka | particle name
 * @date 2011-04-11 | Hirokazu Odaka | derived from AHPrimaryGen (BasicPrimaryGen)
 * @date 2012-07-10 | Hirokazu Odaka | add degree of polarization
 * @date 2013-08-18 | Hirokazu Odaka | v1.4: be moved to anlgeant4
 */
class PlaneWavePrimaryGen : public anlgeant4::BasicPrimaryGen
{
  DEFINE_ANL_MODULE(PlaneWavePrimaryGen, 1.4);
public:
  PlaneWavePrimaryGen();

  virtual anl::ANLStatus mod_startup();
  virtual anl::ANLStatus mod_com();
  virtual anl::ANLStatus mod_init();
  virtual anl::ANLStatus mod_ana();
  virtual anl::ANLStatus mod_endrun();
  
protected:
  G4ThreeVector CenterPosition() { return m_CenterPosition; }
  G4ThreeVector Direction() { return m_Direction0; }

  virtual G4ThreeVector samplePosition();
  virtual G4double GenerationArea();
  
private:
  G4ThreeVector m_CenterPosition;
  
  G4ThreeVector m_Direction0;
  G4ThreeVector m_DirectionOrthogonal;
  G4double m_Radius;

  G4ThreeVector m_Polarization0;
  G4double m_PolarizationDegree;
  
  G4double m_Flux;
};

}

#endif /* ANLGEANT4_PlaneWavePrimaryGen_H */
