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

#ifndef ANLGEANT4_PlaneWavePrimaryGenerator_H
#define ANLGEANT4_PlaneWavePrimaryGenerator_H 1

#include "BasicPrimaryGenerator.hh"
#include "G4ThreeVector.hh"

namespace anlgeant4 {


/**
 * ANLGeant4 PrimaryGenerator module.
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
class PlaneWavePrimaryGenerator : public anlgeant4::BasicPrimaryGenerator
{
  DEFINE_ANL_MODULE(PlaneWavePrimaryGenerator, 4.1);
public:
  PlaneWavePrimaryGenerator();
  ~PlaneWavePrimaryGenerator();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;

  PrimarySetting make_primary_setting() const override;

protected:
  G4ThreeVector sample_position() const override;

  G4ThreeVector center_position() const { return center_position_; }
  G4ThreeVector direction() const { return direction_; }
  G4ThreeVector direction_orthogonal() const { return direction_orthogonal_; }
  virtual double generation_area() const;

private:
  G4ThreeVector center_position_;
  G4ThreeVector direction_;
  G4ThreeVector direction_orthogonal_;
  double radius_;
  G4ThreeVector polarization_;
  double polarization_degree_;
  double flux_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_PlaneWavePrimaryGenerator_H */
