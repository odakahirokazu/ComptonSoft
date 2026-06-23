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

#ifndef ANLGEANT4_GaussianBeamPrimaryGenerator_H
#define ANLGEANT4_GaussianBeamPrimaryGenerator_H 1

#include "PlaneWavePrimaryGenerator.hh"
#include "G4ThreeVector.hh"

namespace anlgeant4 {

/**
 * ANLGeant4 PrimaryGenerator module.
 * Primary particles are generated like a beam that has Gaussian radial profile.
 * @author Hirokazu Odaka
 * @date 2016-01-07 | Hirokazu Odaka | based on PlaneWavePrimaryGen
 */
class GaussianBeamPrimaryGenerator : public anlgeant4::PlaneWavePrimaryGenerator
{
  DEFINE_ANL_MODULE(GaussianBeamPrimaryGenerator, 7.0);
public:
  GaussianBeamPrimaryGenerator();
  ~GaussianBeamPrimaryGenerator();

  anlnext::ANLStatus mod_define() override;

protected:
  G4ThreeVector sample_position() const override;

private:
  double radial_sigma_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_GaussianBeamPrimaryGenerator_H */
