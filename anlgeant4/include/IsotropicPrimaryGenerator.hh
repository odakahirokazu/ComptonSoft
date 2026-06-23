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

#include "PrimarySetting.hh"
#ifndef ANLGEANT4_IsotropicPrimaryGenerator_H
#define ANLGEANT4_IsotropicPrimaryGenerator_H 1

#include "BasicPrimaryGenerator.hh"
#include "G4ThreeVector.hh"

namespace anlgeant4 {


/**
 * ANLGeant4 PrimaryGenerator module.
 * Isotropic and homogeneous particle distribution are realized.
 *
 * @author Hirokazu Odaka
 * @date 2010-xx-xx
 * @date 2017-06-27 | Hirokazu Odaka | 4.1, makePrimarySetting()
 */
class IsotropicPrimaryGenerator : public anlgeant4::BasicPrimaryGenerator
{
  DEFINE_ANL_MODULE(IsotropicPrimaryGenerator, 7.0);
public:
  IsotropicPrimaryGenerator();
  ~IsotropicPrimaryGenerator();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;

  PrimarySetting make_primary_setting() const override;

protected:
  double radius() const { return radius_; }
  double distance() const { return distance_; }
  double covering_factor() const { return covering_factor_; }
  G4ThreeVector center_direction() const { return center_direction_; }
  G4ThreeVector center_position() const { return center_position_; }
  double intensity() const { return intensity_; }

  void set_intensity(double v) { intensity_ = v; }

private:
  G4ThreeVector center_position_;
  double radius_;
  double distance_;
  G4ThreeVector center_direction_;
  double theta_min_;
  double theta_max_;
  double costheta0_;
  double costheta1_;
  double covering_factor_;

  double intensity_; // energy per unit {time, area, solid angle}
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_IsotropicPrimaryGenerator_H */
