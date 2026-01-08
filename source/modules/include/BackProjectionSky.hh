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

#ifndef COMPTONSOFT_BackProjectionSky_H
#define COMPTONSOFT_BackProjectionSky_H 1

#include "BackProjection.hh"

namespace comptonsoft {


/**
 * make back projection image in sky
 * @author Hirokazu Odaka
 * @date 2013-05-20
 */
class BackProjectionSky : public BackProjection
{
  DEFINE_ANL_MODULE(BackProjectionSky, 3.1);
public:
  BackProjectionSky();
  ~BackProjectionSky();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  vector3_t xaxis_;
  vector3_t yaxis_;
  vector3_t zaxis_;

  double image_center_theta_;
  double image_center_phi_;
  double image_yaxis_theta_;
  double image_yaxis_phi_;

  double arm_;
  int num_points_;

};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_BackProjectionSky_H */
