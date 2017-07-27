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

#ifndef ANLGEANT4_NucleusPrimaryGen_H
#define ANLGEANT4_NucleusPrimaryGen_H 1

#include "BasicPrimaryGen.hh"
#include "G4ThreeVector.hh"

class G4ParticleDefinition;

namespace anlgeant4 {


/**
 * ANLGeant4 PrimaryGen module.
 * A nucleus is generated.
 *
 * @author Hirokazu Odaka
 * @date 2011-04-12
 * @date 2016-12-09 | v4.1: migration to geant4 10.3
 * @date 2017-06-30 | v4.2: floating level
 */
class NucleusPrimaryGen : public BasicPrimaryGen
{
  DEFINE_ANL_MODULE(NucleusPrimaryGen, 4.2);
public:
  NucleusPrimaryGen();
  ~NucleusPrimaryGen();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_begin_run() override;

  void makePrimarySetting() override;
  
private:
  G4ThreeVector m_Position0;
  int m_RIZ = 0;
  int m_RIA = 0;
  double m_RIEnergy = 0.0;
  int m_RIFloatingLevel = 0;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_NucleusPrimaryGen_H */
