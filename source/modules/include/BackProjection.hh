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
#include "cs_globals.hh"
#include "EventReconstruction.hh"

namespace comptonsoft {


/**
 * make back projection image
 * @author Hirokazu Odaka
 * @date 2007-xx-xx
 * @date 2012-03-14
 */
class BackProjection : public VCSModule
{
  DEFINE_ANL_MODULE(BackProjection, 2.1);
public:
  BackProjection();
  ~BackProjection() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

protected:
  void SetUnit(double unit, std::string name)
  {
    m_PixelUnit = unit;
    m_PixelUnitName = name;
  }

  double PixelUnit() { return m_PixelUnit; }

  const comptonsoft::TwoHitComptonEvent& GetComptonEvent();
  void FillImage(double x, double y, double weight);
  bool SectionConeAndPlane(const vector3_t& vertex, const vector3_t& cone, vector3_t& coneProjected);
  
private:
  EventReconstruction* m_EventReconstruction;

  vector3_t m_PlaneNormal;
  vector3_t m_PlanePoint;

  TH2D* m_hist_bp_All;
  TH2D* m_hist_bp_All_nf;

  std::vector<TH2D*> m_hist_vec;
  std::vector<TH2D*> m_hist_nf_vec;

  int m_NumPixelX;
  int m_NumPixelY;
  double m_RangeX1;
  double m_RangeX2;
  double m_RangeY1;
  double m_RangeY2;

  double m_PixelUnit;
  std::string m_PixelUnitName;
};

}

#endif /* COMPTONSOFT_BackProjection_H */

