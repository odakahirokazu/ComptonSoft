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

#include "NucleusPrimaryGenInVolume.hh"

using namespace anl;

namespace anlgeant4
{

NucleusPrimaryGenInVolume::NucleusPrimaryGenInVolume()
{
}

NucleusPrimaryGenInVolume::~NucleusPrimaryGenInVolume() = default;

ANLStatus NucleusPrimaryGenInVolume::mod_define()
{
  NucleusPrimaryGen::mod_define();

  register_parameter(&m_VolumeHierarchy, "volume_hierarchy", "seq", "World");
  set_parameter_description("Volume hierarchy that identifies the primary generating volume.");
  
  return AS_OK;
}

ANLStatus NucleusPrimaryGenInVolume::mod_communicate()
{
  unregister_parameter("position");
  return NucleusPrimaryGen::mod_communicate();
}

ANLStatus NucleusPrimaryGenInVolume::mod_begin_run()
{
  anl::ANLStatus status = NucleusPrimaryGen::mod_begin_run();
  if (status != AS_OK) {
    return status;
  }

  m_PositionSampler.setVolumeHierarchy(m_VolumeHierarchy);
  m_PositionSampler.defineVolumeSize();
  
  return AS_OK;
}

void NucleusPrimaryGenInVolume::makePrimarySetting()
{
  G4ThreeVector position(m_PositionSampler.samplePosition());
  
  const double energy = 0.0;
  const G4ThreeVector direction(0.0, 0.0, 0.0);
  setPrimary(position, energy, direction);
}

} /* namespace anlgeant4 */
