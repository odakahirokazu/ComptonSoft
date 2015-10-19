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

#ifndef ANLGEANT4_PlaneWaveRectanglePrimaryGen_H
#define ANLGEANT4_PlaneWaveRectanglePrimaryGen_H 1

#include "PlaneWavePrimaryGen.hh"
#include "G4ThreeVector.hh"

namespace anlgeant4 {

/**
 * ANLGeant4 PrimaryGen module.
 * Primary particles are generated like a plane wave and from a fixed rectangle.
 * @author Hirokazu Odaka
 * @date 2011-06-15 | Hirokazu Odaka | based on PlaneWavePrimaryGen
 * @date 2011-07-10 | Hirokazu Odaka | derived from PlaneWavePrimaryGen
 */
class PlaneWaveRectanglePrimaryGen : public anlgeant4::PlaneWavePrimaryGen
{
  DEFINE_ANL_MODULE(PlaneWaveRectanglePrimaryGen, 4.0);
public:
  PlaneWaveRectanglePrimaryGen();
  ~PlaneWaveRectanglePrimaryGen();

  virtual anl::ANLStatus mod_startup();

protected:
  G4ThreeVector samplePosition();
  G4double GenerationArea();
  
private:
  G4double m_SizeX;
  G4double m_SizeY;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_PlaneWaveRectanglePrimaryGen_H */
