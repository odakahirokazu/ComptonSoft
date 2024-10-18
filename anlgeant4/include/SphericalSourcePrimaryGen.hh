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

#ifndef ANLGEANT4_SphericalSourcePrimaryGen_H
#define ANLGEANT4_SphericalSourcePrimaryGen_H 1

#include "PointSourcePrimaryGen.hh"
#include "G4ThreeVector.hh"

namespace anlgeant4 {


/**
 * ANLGeant4 PrimaryGen module.
 * Primary generator from a spherical source.
 *
 * @author Hirokazu Odaka
 * @date 2024-10-18 | Hirokazu Odaka | 1.0
 */
class SphericalSourcePrimaryGen : public PointSourcePrimaryGen
{
  DEFINE_ANL_MODULE(SphericalSourcePrimaryGen, 1.0);
public:
  SphericalSourcePrimaryGen();
  ~SphericalSourcePrimaryGen();

  anlnext::ANLStatus mod_define() override;

protected:
  G4ThreeVector samplePosition() override;

private:
  double source_inner_radius_;
  double source_outer_radius_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_SphericalSourcePrimaryGen_H */
