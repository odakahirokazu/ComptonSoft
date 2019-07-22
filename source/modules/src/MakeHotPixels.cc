/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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

#include "MakeHotPixels.hh"
#include <fstream>
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft{

MakeHotPixels::MakeHotPixels()
  : filename_("hotpix.txt")
{
}

ANLStatus MakeHotPixels::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}

ANLStatus MakeHotPixels::mod_initialize()
{
  get_module("ConstructFrame", &frame_owner_);
  return AS_OK;
}

ANLStatus MakeHotPixels::mod_end_run()
{
  const comptonsoft::FrameData& frameData = frame_owner_->getFrame();
  const comptonsoft::FrameData::flags_t& hotpixArray = frameData.getHotPixels();
  const int nx = hotpixArray.shape()[0];
  const int ny = hotpixArray.shape()[1];

  std::ofstream outputfile(filename_.c_str());
  for (int ix=0; ix<nx; ix++){
    for (int iy=0; iy<ny; iy++){
      if (hotpixArray[ix][iy]){
        outputfile<<ix;
        outputfile<<" ";
        outputfile<<iy;
        outputfile<<"\n";
      }
    }
  }
  outputfile.close();
  return AS_OK;
}

} /* namespace comptonsoft */
