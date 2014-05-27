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

#include "PrimaryGenInVolume.hh"

#include "G4SystemOfUnits.hh"

using namespace anl;
using namespace comptonsoft;


PrimaryGenInVolume::PrimaryGenInVolume()
  : m_TargetPosition(0.0, 0.0, 0.0)
{
}


ANLStatus PrimaryGenInVolume::mod_startup()
{
  PointSourcePrimaryGen::mod_startup();

  register_parameter(&m_TargetPosition, "Target position", cm, "cm");
  set_parameter_description("Position of the target. The primary direction is distrubuted around the vector toward this target position.");
  register_parameter(&m_VolumeHierarchy, "Volume hierarchy", "seq", "World");
  set_parameter_description("Volume hierarchy that identifies the primary generating volume.");

  return AS_OK;
}


ANLStatus PrimaryGenInVolume::mod_com()
{
  unregister_parameter("Source position");
  unregister_parameter("Center direction");

  return PointSourcePrimaryGen::mod_com();
}


ANLStatus PrimaryGenInVolume::mod_bgnrun()
{
  m_PositionSampler.setVolumeHierarchy(m_VolumeHierarchy);
  m_PositionSampler.defineVolumeSize();
  
  return AS_OK;
}


ANLStatus PrimaryGenInVolume::mod_ana()
{
  G4ThreeVector position(m_PositionSampler.samplePosition());
  setSourcePosition(position);
  G4ThreeVector direction = (m_TargetPosition-position).unit();
  setCenterDirection(direction);
  return PointSourcePrimaryGen::mod_ana();
}
