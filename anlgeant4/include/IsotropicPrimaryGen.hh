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

#ifndef ANLGEANT4_IsotropicPrimaryGen_H
#define ANLGEANT4_IsotropicPrimaryGen_H 1

#include "BasicPrimaryGen.hh"
#include "G4ThreeVector.hh"

namespace anlgeant4 {


/**
 * ANLGeant4 PrimaryGen module.
 * Isotropic and homogeneous particle distribution are realized.
 *
 * @author Hirokazu Odaka
 * @date 2010-xx-xx
 * @date 2017-06-27 | Hirokazu Odaka | 4.1, makePrimarySetting()
 */
class IsotropicPrimaryGen : public anlgeant4::BasicPrimaryGen
{
  DEFINE_ANL_MODULE(IsotropicPrimaryGen, 4.1);
public:
  IsotropicPrimaryGen();
  ~IsotropicPrimaryGen();
  
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;

  void makePrimarySetting() override;

protected:
  double Radius() const { return m_Radius; }
  double Distance() const { return m_Distance; }
  double CoveringFactor() const { return m_CoveringFactor; }
  G4ThreeVector CenterDirection() const { return m_CenterDirection; }
  G4ThreeVector CenterPosition() const { return m_CenterPosition; }
  double Intensity() const { return m_Intensity; }

  void setCoveringFactor (double v) { m_CoveringFactor = v; }
  void setIntensity(double v) { m_Intensity = v; }
  
private:
  G4ThreeVector m_CenterPosition;
  double m_Radius;
  double m_Distance;
  G4ThreeVector m_CenterDirection;
  double m_ThetaMin;
  double m_ThetaMax;
  double m_CosTheta0;
  double m_CosTheta1;
  double m_CoveringFactor;

  double m_Intensity; // energy per unit {time, area, solid angle}
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_IsotropicPrimaryGen_H */
