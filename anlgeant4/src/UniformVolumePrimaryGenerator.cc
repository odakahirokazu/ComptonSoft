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

#include "UniformVolumePrimaryGenerator.hh"

#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

UniformVolumePrimaryGenerator::UniformVolumePrimaryGenerator()
  : target_mode_(false), target_position_(0.0, 0.0, 0.0)
{
}

ANLStatus UniformVolumePrimaryGenerator::mod_define()
{
  PointSourcePrimaryGenerator::mod_define();

  define_parameter("target_mode", &mod_class::target_mode_);
  set_parameter_description("If on, the primary direction is distrubuted around the vector toward the target position.");
  define_parameter("target_position", &mod_class::target_position_, unit::cm, "cm");
  define_parameter("volume_hierarchy", &mod_class::volume_hierarchy_);
  set_parameter_description("Volume hierarchy that identifies the primary generating volume.");

  return AS_OK;
}

ANLStatus UniformVolumePrimaryGenerator::mod_initialize()
{
  PointSourcePrimaryGenerator::mod_initialize();
  undefine_parameter("position");
  if (target_mode_) {
    hide_parameter("direction");
  }
  else {
    hide_parameter("target_position");
  }

  return AS_OK;
}

ANLStatus UniformVolumePrimaryGenerator::mod_begin_run()
{
  PointSourcePrimaryGenerator::mod_begin_run();
  position_sampler_.set_volume_hierarchy(volume_hierarchy_);
  position_sampler_.define_volume_size();

  return AS_OK;
}

G4ThreeVector UniformVolumePrimaryGenerator::sample_position() const
{
  return position_sampler_.sample_position();
}

PrimarySetting UniformVolumePrimaryGenerator::make_primary_setting() const
{
  const double energy = sample_energy();
  const G4ThreeVector position = sample_position();

  G4ThreeVector direction;
  if (target_mode_) {
    direction = sample_direction_from_cosine_range();
    const G4ThreeVector center_direction = (target_position_-position).unit();
    direction.rotateUz(center_direction);
  }
  else {
    direction = sample_direction();
  }

  G4ThreeVector polarization = unpolarized_vector(direction);

  return make_primary(position, energy, direction, polarization);
}

} /* namespace anlgeant4 */
