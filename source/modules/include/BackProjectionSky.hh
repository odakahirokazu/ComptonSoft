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
  DEFINE_ANL_MODULE(BackProjectionSky, 2.2);
public:
  BackProjectionSky();
  ~BackProjectionSky();

  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_ana() override;

private:
  double m_Rotation;
  vector3_t m_XAxis;
  vector3_t m_YAxis;
  vector3_t m_ZAxis;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_BackProjectionSky_H */
