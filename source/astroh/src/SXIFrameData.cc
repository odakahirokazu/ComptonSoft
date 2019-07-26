/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka, Tsubasa Tamba                      *
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

#include <fstream>
#include <iostream>
#include <algorithm>
#include "SXIFrameData.hh"

namespace comptonsoft
{

SXIFrameData::SXIFrameData(int nx, int ny)
  : FrameData(nx, ny),
    outerSplitThreshold_(0.0),
    surroundThreshold_(0.05475),
    npixSurroundThreshold_(8)
{
}

SXIFrameData::~SXIFrameData() = default;

std::vector<XrayEvent_sptr> SXIFrameData::extractEvents()
{
  image_t &sxiFrame = getFrame();
  std::vector<XrayEvent_sptr> events;

  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  const int margin = 2;
  const int innerSize = 3;
  const int outerSize = 5;

  std::list<std::pair<int, int>> hitPixels;
  for (int iy=margin; iy<ny-margin; iy++) {
    for (int ix=margin; ix<nx-margin; ix++) {
      const double v = sxiFrame[ix][iy];
      if (v > EventThreshold()) {
        if (isMaxPixel(ix, iy, innerSize) && !surroundDiscri(sxiFrame, ix, iy, innerSize, SurroundThreshold(), NpixSurroundThreshold())) {
          hitPixels.emplace_back(ix, iy);
        }
      }
    }
  }

  for (const auto& pixel: hitPixels) {
    const std::size_t ix = pixel.first;
    const std::size_t iy = pixel.second;  
    SXIXrayEvent_sptr event(new SXIXrayEvent(outerSize));
    outerSplitThreshold_ = SplitThreshold();
    event -> setSplitThreshold(SplitThreshold());
    event -> setOuterSplitThreshold(OuterSplitThreshold());
    event -> setOuterSplitThreshold(OuterSplitThreshold());
    event -> copyFrom(sxiFrame, ix, iy);
    event -> reduce();
    events.push_back(event);
  }

  return events;
}

bool SXIFrameData::surroundDiscri(image_t& frame, int ix, int iy, int size, double surroundThreshold, int npixSurroundThreshold)
{
  const int halfSize = size/2;
  const int ix0 = ix-halfSize;
  const int iy0 = iy-halfSize;
  int npix = 0;

  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      const double v = frame[ix0+i][iy0+j];
      if (ix0+i!=ix || iy0+j!=iy) {
        if (v >= surroundThreshold) {
          npix+=1;
        }
      }
    }
  }

  if (npix >= npixSurroundThreshold) {
    return true;
  }

  return false;
}

} /* namespace comptonsoft */
