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

namespace comptonsoft
{

FrameData::FrameData(const int nx, const int ny)
  : num_pixels_x_(nx),
    num_pixels_y_(ny),
    margin_(10),
    rawFrame_(boost::extents[nx][ny]),
    frame_(boost::extents[nx][ny]),
    pedestals_(boost::extents[nx][ny]),
    weight_(boost::extents[nx][ny]),
    sum_(boost::extents[nx][ny]),
    sum2_(boost::extents[nx][ny]),
    hotpix_(boost::extents[nx][ny])
{
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      sum_[i][j] = 0.0;
      sum2_[i][j] = 0.0;
      hotpix_[i][j] = false;
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
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  const int margin = Margin();
  const int size = EventSize();

  if (size>=margin) {
    std::cout << "Event size is larger than the pixel margin." << std::endl;
    return events;
  }

  std::list<std::pair<int, int>> hitPixels;
  for (int ix=margin; ix<nx-margin; ix++) {
    for (int iy=margin; iy<ny-margin; iy++) {
      const double v = frame_[ix][iy];
      if (!hotpix_[ix][iy] && v>=EventThreshold()) {
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

void FrameData::detectHotPixels(double threshold)
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++){
      if (rawFrame_[ix][iy] > threshold){
        hotpix_[ix][iy] = true;
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
      if (hotpix_[ix0+i][iy0+j]) {
        return true;
      }
    }
  }
  return false;
}

} /* namespace comptonsoft */
