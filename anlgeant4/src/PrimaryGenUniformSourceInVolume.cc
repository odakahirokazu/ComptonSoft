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

#include "PrimaryGenUniformSourceInVolume.hh"

#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

PrimaryGenUniformSourceInVolume::PrimaryGenUniformSourceInVolume()
  : m_TargetMode(false), m_TargetPosition(0.0, 0.0, 0.0)
{
}

ANLStatus PrimaryGenUniformSourceInVolume::mod_define()
{
  PointSourcePrimaryGen::mod_define();

  register_parameter(&m_TargetMode, "target_mode");
  set_parameter_description("If on, the primary direction is distrubuted around the vector toward the target position.");
  register_parameter(&m_TargetPosition, "target_position", unit::cm, "cm");
  register_parameter(&m_VolumeHierarchy, "volume_hierarchy");
  set_parameter_description("Volume hierarchy that identifies the primary generating volume.");

  return AS_OK;
}

ANLStatus PrimaryGenUniformSourceInVolume::mod_initialize()
{
  PointSourcePrimaryGen::mod_initialize();
  unregister_parameter("position");
  if (m_TargetMode) {
    hide_parameter("direction");
    setCenterDirection(G4ThreeVector{0.0, 0.0, 1.0});
  }
  else {
    hide_parameter("target_position");
  }

  return AS_OK;
}

ANLStatus PrimaryGenUniformSourceInVolume::mod_begin_run()
{
  m_PositionSampler.setVolumeHierarchy(m_VolumeHierarchy);
  m_PositionSampler.defineVolumeSize();

  return AS_OK;
}

PrimarySetting PrimaryGenUniformSourceInVolume::make_primary_setting() const
{
  const double energy = sample_energy();

  const PositionSamplerInVolume& position_sampler = m_PositionSampler;
  const G4ThreeVector position = position_sampler.samplePosition();

  G4ThreeVector direction = sample_direction();
  if (m_TargetMode) {
    const G4ThreeVector center_direction = (m_TargetPosition-position).unit();
    direction.rotateUz(center_direction);
  }

  return PrimarySetting{energy, direction, 0.0, position, unpolarized_vector(direction)};
}

} /* namespace anlgeant4 */
