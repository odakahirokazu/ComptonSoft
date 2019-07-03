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

#ifndef COMPTONSOFT_BackProjection_H
#define COMPTONSOFT_BackProjection_H 1

#include "VCSModule.hh"

#include "TH2.h"
#include "CSTypes.hh"
#include "EventReconstruction.hh"

namespace comptonsoft {

/**
 * make back projection image
 * @author Hirokazu Odaka
 * @date 2007-xx-xx
 * @date 2012-03-14
 * @date 2019-07-03 | remove CdTeFluor flag
 */
class BackProjection : public VCSModule
{
  DEFINE_ANL_MODULE(BackProjection, 2.3);
public:
  BackProjection();
  ~BackProjection();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  void setUnit(double unit, std::string name)
  {
    m_PixelUnit = unit;
    m_PixelUnitName = name;
  }

  double PixelUnit() { return m_PixelUnit; }

  const comptonsoft::BasicComptonEvent& getComptonEvent();
  void fillImage(double x, double y, double weight);
  bool sectionConeAndPlane(const vector3_t& vertex, const vector3_t& cone, vector3_t& coneProjected);
  
private:
  EventReconstruction* m_EventReconstruction;

  vector3_t m_PlaneNormal;
  vector3_t m_PlanePoint;

  TH2D* m_hist_bp_All;
  std::vector<TH2D*> m_hist_vec;

  int m_NumPixelX;
  int m_NumPixelY;
  double m_RangeX1;
  double m_RangeX2;
  double m_RangeY1;
  double m_RangeY2;

  double m_PixelUnit;
  std::string m_PixelUnitName;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_BackProjection_H */
