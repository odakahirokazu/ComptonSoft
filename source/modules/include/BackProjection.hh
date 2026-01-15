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
 * @date 2026-01-15 | ARM spread
 */
class BackProjection : public VCSModule
{
  DEFINE_ANL_MODULE(BackProjection, 4.0);
public:
  BackProjection();
  ~BackProjection();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  void setUnit(double unit, std::string name)
  {
    pixel_unit_ = unit;
    pixel_unit_name_ = name;
  }
  double PixelUnit() { return pixel_unit_; }

  std::size_t NumPoints() const { return static_cast<std::size_t>(num_points_); }
  double ARMSpread() const { return arm_spread_; }

  void fillImage(double x, double y, double weight);
  bool sectionConeAndPlane(const vector3_t& vertex, const vector3_t& cone, vector3_t& cone_section);

  EventReconstruction* getEventReconstructionModule()
  { return event_reconstruction_module_; }
  
private:
  EventReconstruction* event_reconstruction_module_;

  vector3_t plane_normal_;
  vector3_t plane_point_;

  TH2D* hist_bp_all_;
  std::vector<TH2D*> hist_vec_;

  int num_pixel_x_;
  int num_pixel_y_;
  double range_x1_;
  double range_x2_;
  double range_y1_;
  double range_y2_;

  double pixel_unit_;
  std::string pixel_unit_name_;

  int num_points_;
  double arm_spread_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_BackProjection_H */
