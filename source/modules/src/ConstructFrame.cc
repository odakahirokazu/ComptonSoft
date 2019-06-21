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

#include "ConstructFrame.hh"
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft {

ConstructFrame::ConstructFrame()
{
  add_alias("ConstructFrame");
}

ANLStatus ConstructFrame::mod_define()
{
  define_parameter("num_pixel_x", &mod_class::num_pixel_x_);
  define_parameter("num_pixel_y", &mod_class::num_pixel_y_);
  
  return AS_OK;
}

ANLStatus ConstructFrame::mod_initialize()
{
  frame_.reset(createFrameData());
  frame_->setPedestals(0.0);

  return AS_OK;
}

FrameData* ConstructFrame::createFrameData()
{
  return new comptonsoft::FrameData(num_pixel_x_, num_pixel_y_);
}

} /* namespace comptonsoft */
