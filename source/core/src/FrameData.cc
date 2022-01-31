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

#include "VGainFunction.hh"
#include "GainFunctionLinear.hh"

namespace comptonsoft
{

FrameData::FrameData(const int nx, const int ny)
  : num_pixels_x_(nx),
    num_pixels_y_(ny),
    startPosition_(CornerID::BottomLeft),
    rawFrame_(boost::extents[nx][ny]),
    frame_(boost::extents[nx][ny]),
    pedestals_(boost::extents[nx][ny]),
    weight_(boost::extents[nx][ny]),
    sum_(boost::extents[nx][ny]),
    sum2_(boost::extents[nx][ny]),
    deviation_(boost::extents[nx][ny]),
    disabledPixels_(boost::extents[nx][ny]),
    pixelValuesToExclude_(boost::extents[nx][ny])
{
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      sum_[ix][iy] = 0.0;
      sum2_[ix][iy] = 0.0;
      deviation_[ix][iy] = 0.0;
      disabledPixels_[ix][iy] = 0;
      pixelValuesToExclude_[ix][iy] = std::make_unique<OutlierStore>();
      addEventCheckPixels(ix, iy);
    }
  }
  buf_.resize(2*nx*ny);

  commonGainFunction_ = std::make_shared<GainFunctionLinear>(0.0, 1.0);
}

FrameData::~FrameData() = default;

void FrameData::setStatisticsExclusionNumbers(int num_low, int num_high)
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      pixelValuesToExclude_[ix][iy]->set_capacities(num_low, num_high);
    }
  }
}

void FrameData::resetRawFrame()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      rawFrame_[ix][iy] = 0.0;
    }
  }
}

bool FrameData::load(const std::string& filename)
{
  std::ifstream infile;
  const std::size_t length = buf_.size();

  infile.open(filename);
  if (!infile) {
    std::cerr << " cannot open file: " << filename << std::endl;
    return false;
  }

  infile.read(&buf_[0], length);
  infile.close();

  const int odd_row_pixel_shift = OddRowPixelShift();
  const bool regular_pixel_arrangement = (odd_row_pixel_shift == 0);

  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      int t = -1;
      if (!readDirectionX_) {
        if (startPosition_==CornerID::BottomLeft)  { t = ix*ny+iy; }
        if (startPosition_==CornerID::UpperLeft)   { t = ix*ny+(ny-1-iy); }
        if (startPosition_==CornerID::BottomRight) { t = (nx-1-ix)*ny+iy; }
        if (startPosition_==CornerID::UpperRight)  { t = (nx-1-ix)*ny+(ny-1-iy); }
      }
      else {
        if (startPosition_==CornerID::BottomLeft)  { t = iy*nx+ix; }
        if (startPosition_==CornerID::UpperLeft)   { t = (ny-1-iy)*nx+ix; }
        if (startPosition_==CornerID::BottomRight) { t = iy*nx+(nx-1-ix); }
        if (startPosition_==CornerID::UpperRight)  { t = (ny-1-iy)*nx+(nx-1-ix); }
      }

      unsigned int v = 0u;
      if (ByteOrder()) {
        v = ((static_cast<uint16_t>(buf_[2*t])&0xff)<<8) + (static_cast<uint16_t>(buf_[2*t+1])&0xff);
      }
      else {
        v = ((static_cast<uint16_t>(buf_[2*t+1])&0xff)<<8) + (static_cast<uint16_t>(buf_[2*t])&0xff);
      }

      if (regular_pixel_arrangement) {
        rawFrame_[ix][iy] = v;
      }
      else {
        const int pixel_shift = (iy%2==0) ? 0 : odd_row_pixel_shift;
        const int ix_shifted = ix + pixel_shift;
        if (0<=ix_shifted && ix_shifted<ny) {
          rawFrame_[ix_shifted][iy] = v;
        }
      }
    }
  }
  return true;
}

void FrameData::stack()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      const double v = rawFrame_[ix][iy];
      weight_[ix][iy] += 1.0;
      sum_[ix][iy] += v;
      sum2_[ix][iy] += v*v;
      pixelValuesToExclude_[ix][iy]->propose(v);
    }
  }
}

void FrameData::setPedestals(const double v)
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      pedestals_[ix][iy] = v;
    }
  }
}

void FrameData::calculateStatistics()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      const double w = weight_[ix][iy] - pixelValuesToExclude_[ix][iy]->num();
      const double sum = sum_[ix][iy] - pixelValuesToExclude_[ix][iy]->sum();
      const double sum2 = sum2_[ix][iy] - pixelValuesToExclude_[ix][iy]->sum2();
      if (w != 0.0) {
        pedestals_[ix][iy] = sum/w;
        deviation_[ix][iy] = sqrt(sum2/w - (sum*sum)/(w*w));
      }
    }
  }
}

void FrameData::subtractPedestals()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      frame_[ix][iy] = rawFrame_[ix][iy] - pedestals_[ix][iy];
    }
  }
}

void FrameData::correctGains()
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      const double pha = frame_[ix][iy];
      const double energy = correctGain(ix, iy, pha);
      frame_[ix][iy] = energy;
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
  for (std::size_t i=0; i<eventCheckPixels_.size(); i++){
    const int ix = eventCheckPixels_[i].first;
    const int iy = eventCheckPixels_[i].second;
    const double v = frame_[ix][iy];
    if (ix>=margin && ix<nx-margin && iy>=margin && iy<ny-margin) {
      if (isNotDisabledPixel(ix, iy) && v>=EventThreshold()) {
        if (isMaxPixel(ix, iy, size) && !(includeDisabledPixel(ix, iy, size))) {
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
  for (int jx=0; jx<size; jx++) {
    for (int jy=0; jy<size; jy++) {
      const double v = frame_[ix0+jx][iy0+jy];
      if (ix0+jx!=ix || iy0+jy!=iy) {
        if (v >= vCenter) {
          return false;
        }
      }
    }
  }
  return true;
}

void FrameData::selectGoodPixels(const double mean_min, const double mean_max,
                                 const double sigma_min, const double sigma_max)
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++){
      const double mean = pedestals_[ix][iy];
      const double sigma = deviation_[ix][iy];
      if (mean<mean_min || mean_max<mean
          || sigma<sigma_min || sigma_max<sigma) {
        disabledPixels_[ix][iy] = 1;
      }
      else {
        disabledPixels_[ix][iy] = 0;
      }
    }
  }
}

bool FrameData::includeDisabledPixel(int ix, int iy, int size) const
{
  const int halfSize = size/2;
  const int ix0 = ix-halfSize;
  const int iy0 = iy-halfSize;
 
  for (int jx=0; jx<size; jx++) {
    for (int jy=0; jy<size; jy++) {
      if (disabledPixels_[ix0+jx][iy0+jy]) {
        return true;
      }
    }
  }
  return false;
}

double FrameData::RawFrameMedian() const
{
  const int nx = NumPixelsX();
  const int ny = NumPixelsY();
  std::vector<double> v;
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      if (isNotDisabledPixel(ix, iy)) {
        v.push_back(rawFrame_[ix][iy]);
      }
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

void FrameData::resetGainFunctions()
{
  shareGainFunction_ = true;
  commonGainFunction_ = std::make_shared<GainFunctionLinear>(0.0, 1.0);
  gainFunctions_.reset();
}

void FrameData::setCommonGainFunction(const std::shared_ptr<VGainFunction>& f)
{
  commonGainFunction_ = f;
}

void FrameData::setGainFunction(int ix, int iy, const std::shared_ptr<VGainFunction>& f)
{
  if (shareGainFunction_) {
    shareGainFunction_ = false;
    const int nx = num_pixels_x_;
    const int ny = num_pixels_y_;
    gainFunctions_ = std::make_shared<gain_func_array_t>(boost::extents[nx][ny]);
    for (int kx=0; kx<nx; kx++) {
      for (int ky=0; ky<ny; ky++) {
        (*gainFunctions_)[kx][ky] = commonGainFunction_;
      }
    }
  }

  (*gainFunctions_)[ix][iy] = f;
}

VGainFunction* FrameData::getGainFunction(int ix, int iy) const
{
  VGainFunction* f = nullptr;
  if (shareGainFunction_) {
    return commonGainFunction_.get();
  }
  else {
    if ((f = (*gainFunctions_)[ix][iy].get()) != nullptr) {
      return f;
    }
    else {
      return commonGainFunction_.get();
    }
  }
  return nullptr;
}

double FrameData::correctGain(int ix, int iy, double pha) const
{
  return getGainFunction(ix, iy)->eval(pha);
}

} /* namespace comptonsoft */
