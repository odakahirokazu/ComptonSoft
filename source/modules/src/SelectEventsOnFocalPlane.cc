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

#include "SelectEventsOnFocalPlane.hh"
#include "AstroUnits.hh"
#include "BasicComptonEvent.hh"
#include "EventReconstruction.hh"

using namespace anl;

namespace comptonsoft
{

SelectEventsOnFocalPlane::SelectEventsOnFocalPlane()
  : m_DetectorID(0),
    m_RegionTypeString("rectangle"),
    m_RegionType(Region_t::Rectangle),
    m_Center(0.0, 0.0),
    m_SizeX(0.0),
    m_SizeY(0.0),
    m_Radius(0.0),
    m_Radius2(0.0)
{
}

ANLStatus SelectEventsOnFocalPlane::mod_startup()
{
  register_parameter(&m_DetectorID, "detector_id");
  register_parameter(&m_RegionTypeString, "region_type");
  register_parameter(&m_Center, "center", cm, "cm");
  register_parameter(&m_SizeX, "size_x", cm, "cm");
  register_parameter(&m_SizeY, "size_y", cm, "cm");
  register_parameter(&m_Radius, "radius", cm, "cm");
  return AS_OK;
}

ANLStatus SelectEventsOnFocalPlane::mod_init()
{
  VCSModule::mod_init();
 
  GetModule("EventReconstruction", &m_EventReconstruction);

  if (m_RegionTypeString=="rectangle") {
    m_RegionType = Region_t::Rectangle;
  }
  else if (m_RegionTypeString=="circle") {
    m_RegionType = Region_t::Circle;
  }
  else {
    std::cout << "Unknown region type: " << m_RegionTypeString << std::endl;
    return AS_QUIT_ERR;
  }
  
  m_Radius2 = m_Radius * m_Radius;
  
  return AS_OK;
}

ANLStatus SelectEventsOnFocalPlane::mod_ana()
{
  const BasicComptonEvent& comptonEvent = m_EventReconstruction->getComptonEvent();

  if (comptonEvent.Hit1DetectorID() != m_DetectorID) {
    return AS_OK;
  }
  
  const double x = comptonEvent.Hit1PositionX();
  const double y = comptonEvent.Hit1PositionY();

  if (m_RegionType==Region_t::Rectangle) {
    const double x0 = m_Center.x() - 0.5*m_SizeX;
    const double x1 = m_Center.x() + 0.5*m_SizeX;
    if (x < x0 || x1 < x) {
      return AS_SKIP;
    }

    const double y0 = m_Center.y() - 0.5*m_SizeY;
    const double y1 = m_Center.y() + 0.5*m_SizeY;
    if (y < y0 || y1 < y) {
      return AS_SKIP;
    }
  }
  else if (m_RegionType==Region_t::Circle) {
    const double dx = x-m_Center.x();
    const double dy = y-m_Center.y();
    if (dx*dx+dy*dy > m_Radius2) {
      return AS_SKIP;
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
