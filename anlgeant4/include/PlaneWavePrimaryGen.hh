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
 * @date 2017-06-27 | Hirokazu Odaka | 4.1, makePrimarySetting()
 */
class PlaneWavePrimaryGen : public anlgeant4::BasicPrimaryGen
{
  DEFINE_ANL_MODULE(PlaneWavePrimaryGen, 4.1);
public:
  PlaneWavePrimaryGen();
  ~PlaneWavePrimaryGen();

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_communicate() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_end_run() override;
  
  void makePrimarySetting() override;

protected:
  G4ThreeVector CenterPosition() const { return m_CenterPosition; }
  G4ThreeVector Direction() const { return m_Direction0; }
  G4ThreeVector DirectionOrthogonal() const { return m_DirectionOrthogonal; }

  G4ThreeVector samplePosition() override;
  virtual double GenerationArea();
  
private:
  G4ThreeVector m_CenterPosition;
  
  G4ThreeVector m_Direction0;
  G4ThreeVector m_DirectionOrthogonal;
  double m_Radius;

  G4ThreeVector m_Polarization0;
  double m_PolarizationDegree;
  
  double m_Flux;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_PlaneWavePrimaryGen_H */
