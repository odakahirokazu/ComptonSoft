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

namespace comptonsoft
{

NucleusPrimaryGenInVolume::NucleusPrimaryGenInVolume()
{
}

ANLStatus NucleusPrimaryGenInVolume::mod_startup()
{
  NucleusPrimaryGen::mod_startup();

  register_parameter(&m_VolumeHierarchy, "volume_hierarchy", "seq", "World");
  set_parameter_description("Volume hierarchy that identifies the primary generating volume.");
  
  return AS_OK;
}

ANLStatus NucleusPrimaryGenInVolume::mod_com()
{
  unregister_parameter("position");
  return NucleusPrimaryGen::mod_com();
}

ANLStatus NucleusPrimaryGenInVolume::mod_bgnrun()
{
  anl::ANLStatus status = NucleusPrimaryGen::mod_bgnrun();
  if (status != AS_OK) {
    return status;
  }

  m_PositionSampler.setVolumeHierarchy(m_VolumeHierarchy);
  m_PositionSampler.defineVolumeSize();
  
  return AS_OK;
}

ANLStatus NucleusPrimaryGenInVolume::mod_ana()
{
  G4ThreeVector position(m_PositionSampler.samplePosition());
  
  G4double energy = 0.0;
  G4ThreeVector direction(0.0, 0.0, 0.0);
  setPrimary(position, energy, direction);
  
  return NucleusPrimaryGen::mod_ana();
}

} /* namespace comptonsoft */
