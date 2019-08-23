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

#include "FrameData.hh"

#include <fstream>
#include <iostream>
#include <cstdint>
#include <list>
#include <algorithm>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

namespace comptonsoft
{

FrameData::FrameData(const int nx, const int ny)
  : num_pixels_x_(nx),
    num_pixels_y_(ny),
    rawFrame_(boost::extents[nx][ny]),
    frame_(boost::extents[nx][ny]),
    pedestals_(boost::extents[nx][ny]),
    weight_(boost::extents[nx][ny]),
    sum_(boost::extents[nx][ny]),
    sum2_(boost::extents[nx][ny]),
    deviation_(boost::extents[nx][ny]),
    hotPixels_(boost::extents[nx][ny])
{
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      sum_[i][j] = 0.0;
      sum2_[i][j] = 0.0;
      deviation_[i][j] = 0.0;
      hotPixels_[i][j] = false;
    }
  }
  buf_.resize(2*nx*ny);
}

FrameData::~FrameData() = default;

void FrameData::resetRawFrame()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      rawFrame_[i][j] = 0.0;
    }
  }
}

bool FrameData::load(const std::string& filename)
{
  std::ifstream infile;
  const std::size_t length = buf_.size();

  infile.open(filename);
  if (!infile) {
    std::cerr << " cannot open file "<< std::endl;
    return false;
  }

  infile.read(&buf_[0], length);
  infile.close();

  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      const int t = i*ny+j;
      const unsigned int v = ((static_cast<uint16_t>(buf_[2*t])&0xff)<<8) + (static_cast<uint16_t>(buf_[2*t+1])&0xff);
      rawFrame_[i][j] = v;
    }
  }
  return true;
}

void FrameData::stack()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      const double v = rawFrame_[i][j];
      const double threshold = pedestals_[i][j]+EventThreshold();
      if (v < threshold) {
        weight_[i][j] += 1.0;
        sum_[i][j] += v;
        sum2_[i][j] += v*v;
      }
    }
  }
}

void FrameData::setPedestals(const double v)
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      pedestals_[i][j] = v;
    }
  }
}

void FrameData::calculateDeviation()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      const double w = weight_[i][j];
      if (w != 0.0) {
        deviation_[i][j] = sqrt(sum2_[i][j]/w - (sum_[i][j]*sum_[i][j])/(w*w));
      }
    }
  }
}

void FrameData::calculatePedestals()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      const double w = weight_[i][j];
      if (w != 0.0) {
        pedestals_[i][j] = sum_[i][j]/w;
      }
    }
  }
}

void FrameData::subtractPedestals()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      frame_[i][j] = rawFrame_[i][j] - pedestals_[i][j];
    }
  }
}

std::vector<XrayEvent_sptr> FrameData::extractEvents()
{
  std::vector<XrayEvent_sptr> events;
  if (badFrame_) {
    return events;
  }

  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  const int size = EventSize();
  const int margin = size/2+TrimSize();

  std::list<std::pair<int, int>> hitPixels;
  for (int ix=margin; ix<nx-margin; ix++) {
    for (int iy=margin; iy<ny-margin; iy++) {
      const double v = frame_[ix][iy];
      if (!hotPixels_[ix][iy] && v>=EventThreshold()) {
        if (isMaxPixel(ix, iy, size) && !(includeHotPixel(ix, iy, size))) {
          hitPixels.emplace_back(ix, iy);
        }
      }
    }
  }
  
  for (const auto& pixel: hitPixels) {
    const int ix = pixel.first;
    const int iy = pixel.second;
    XrayEvent_sptr event(new XrayEvent(size));
    event->setSplitThreshold(SplitThreshold());
    event->copyFrom(frame_, ix, iy);
    event->reduce();
    events.push_back(event);
  }

  return events;
}

bool FrameData::isMaxPixel(int ix, int iy, int size) const
{
  const int halfSize = size/2;
  const int ix0 = ix-halfSize;
  const int iy0 = iy-halfSize;

  const double vCenter = frame_[ix][iy];
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      const double v = frame_[ix0+i][iy0+j];
      if (ix0+i!=ix || iy0+j!=iy) {
        if (v >= vCenter) {
          return false;
        }
      }
    }
  }
  return true;
}

void FrameData::detectHotPixels()
{
  const double threshold = HotPixelThreshold();
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++){
      if (rawFrame_[ix][iy] > threshold){
        hotPixels_[ix][iy] = true;
      }
    }
  }
}

bool FrameData::includeHotPixel(int ix, int iy, int size) const
{
  const int halfSize = size/2;
  const int ix0 = ix-halfSize;
  const int iy0 = iy-halfSize;
 
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      if (hotPixels_[ix0+i][iy0+j]) {
        return true;
      }
    }
  }
  return false;
}

double FrameData::rawFrameMedian() const
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  std::vector<double> v;
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      v.push_back(rawFrame_[ix][iy]);
    }
  }
  std::sort(v.begin(), v.end());
  const int size = v.size();
  if (size%2==1) {
    return v[size/2];
  }
  else {
    return 0.5*(v[size/2]+v[size/2-1]);
  }
}

} /* namespace comptonsoft */
