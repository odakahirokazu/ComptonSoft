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

#ifndef COMPTONSOFT_SelectEventsOnFocalPlane_H
#define COMPTONSOFT_SelectEventsOnFocalPlane_H 1

#include "VCSModule.hh"
#include "G4TwoVector.hh"

namespace comptonsoft {

class EventReconstruction;

/**
 *
 * @author Hirokazu Odaka
 * @date 2016-10-06
 */
class SelectEventsOnFocalPlane : public VCSModule
{
  DEFINE_ANL_MODULE(SelectEventsOnFocalPlane, 1.0);
public:
  enum class Region_t { Rectangle, Circle };

public:
  SelectEventsOnFocalPlane();
  ~SelectEventsOnFocalPlane() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  const EventReconstruction* m_EventReconstruction = nullptr;
  int m_DetectorID;
  std::string m_RegionTypeString;
  Region_t m_RegionType;
  G4TwoVector m_Center;
  double m_SizeX;
  double m_SizeY;
  double m_Radius;
  double m_Radius2;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SelectEventsOnFocalPlane_H */
