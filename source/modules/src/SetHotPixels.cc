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

#include "SetHotPixels.hh"
#include "FrameData.hh"
#include <fstream>

using namespace anlnext;

namespace comptonsoft{

SetHotPixels::SetHotPixels()
  : filename_("hotpix.txt")
{
}

ANLStatus SetHotPixels::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}

ANLStatus SetHotPixels::mod_initialize()
{
  get_module_NC("ConstructFrame", &frame_owner_);
  return AS_OK;
}

ANLStatus SetHotPixels::mod_begin_run()
{
  std::vector<std::pair<int, int>> hotPixels;
  std::ifstream fin(filename_);
  int x=0, y=0;
  while (fin >> x >> y) {
    hotPixels.emplace_back(x, y);
  }
  fin.close();

  FrameData& frameData = frame_owner_->getFrame();
  FrameData::flags_t& array = frameData.getHotPixels();
  const int nx = array.shape()[0];
  const int ny = array.shape()[1];

  for (int ix=0; ix<nx; ix++){
    for (int iy=0; iy<ny; iy++){
      array[ix][iy] = false;
    }
  }

  for (const auto& pair: hotPixels) {
    const int x = pair.first;
    const int y = pair.second;
    array[x][y] = true;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
