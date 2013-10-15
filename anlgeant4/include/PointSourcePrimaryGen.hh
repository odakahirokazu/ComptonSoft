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

#ifndef ANLGEANT4_PointSourcePrimaryGen_H
#define ANLGEANT4_PointSourcePrimaryGen_H 1

#include "BasicPrimaryGen.hh"
#include "G4ThreeVector.hh"

namespace anlgeant4 {


/**
 * ANLGeant4 PrimaryGen module.
 * Primary particles are generated at a fixed point.
 *
 * @author Shin Watanabe, Hirokazu Odaka
 * @date xxxx-xx-xx | Shin Watanabe | NSPrimaryGen
 * @date 2010-02-17 | Hirokazu Odaka | PointSourcePrimaryGen
 * @date 2012-07-10 | Hirokazu Odaka | virtual methods: sampleDirection(), samplePosition()
 * @date 2013-08-18 | Hirokazu Odaka | be moved to anlgeant4
 */
class PointSourcePrimaryGen : public BasicPrimaryGen
{
  DEFINE_ANL_MODULE(PointSourcePrimaryGen, 1.6);
public:
  PointSourcePrimaryGen();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_endrun();

protected:
  void setSourcePosition(G4ThreeVector v) { m_SourcePosition = v; }
  void setCenterDirection(G4ThreeVector v) { m_CenterDirection = v; }
  void calculateRotation();

  G4ThreeVector sampleDirection();
  G4ThreeVector samplePosition();

  G4ThreeVector SourcePosition() { return m_SourcePosition; }

private:
  G4ThreeVector m_SourcePosition;
  
  G4ThreeVector m_CenterDirection;
  G4double m_Theta0;
  G4double m_Theta1;
  G4double m_CosTheta0;
  G4double m_CosTheta1;
  G4double m_CoveringFactor;
  
  G4double m_Luminosity;
};

}

#endif /* ANLGEANT4_PointSourcePrimaryGen_H */
