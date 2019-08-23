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

#ifndef COMPTONSOFT_FrameData_H
#define COMPTONSOFT_FrameData_H 1

#include <string>
#include <vector>
#include "XrayEvent.hh"

namespace comptonsoft
{

/**
 * A class of a frame measured with a frame-readout pixel detector.
 *
 * @author Hirokazu Odaka
 * @date 2019-05-22
 */
class FrameData
{
public:
  using flags_t = boost::multi_array<bool, 2>;

public:
  FrameData(int nx, int ny);
  virtual ~FrameData();
  
  FrameData(const FrameData& r) = default;
  FrameData(FrameData&& r) = default;
  FrameData& operator=(const FrameData& r) = default;
  FrameData& operator=(FrameData&& r) = default;

  int NumPixelsX() const { return num_pixels_x_; }
  int NumPixelsY() const { return num_pixels_y_; }

  void setEventSize(int v) { eventSize_ = v; }
  int EventSize() const { return eventSize_; }

  void setEventThreshold(double v) { eventThreshold_ = v; }
  void setSplitThreshold(double v) { splitThreshold_ = v; }
  double EventThreshold() const { return eventThreshold_; }
  double SplitThreshold() const { return splitThreshold_; }
  void setHotPixelThreshold(double v) { hotPixelThreshold_ = v; }
  double HotPixelThreshold() { return hotPixelThreshold_; }

  void resetRawFrame();
  virtual bool load(const std::string& filename);
  void stack();

  void setPedestals(double v);
  virtual void calculatePedestals();
  virtual void calculateDeviation();

  virtual void subtractPedestals();
  virtual std::vector<XrayEvent_sptr> extractEvents();

  void detectHotPixels();

  void setRawFrame(const image_t& v) { rawFrame_ = v; }
  void setFrame(const image_t& v) { frame_ = v; }
  const image_t& getRawFrame() const { return rawFrame_; }
  image_t& getRawFrame() { return rawFrame_; }
  const image_t& getFrame() const { return frame_; }
  image_t& getFrame() { return frame_; }
  const image_t& getPedestals() const { return pedestals_; }
  image_t& getPedestals() { return pedestals_; }

  const image_t& getWeight() const { return weight_; }
  image_t& getWeight() { return weight_; }
  const image_t& getSumFrame() const { return sum_; }
  image_t& getSumFrame() { return sum_; }
  const image_t& getSum2Frame() const { return sum2_; }
  image_t& getSum2Frame() { return sum2_; }
  const image_t& getDeviationFrame() const { return deviation_; }
  image_t& getDeviationFrame() { return deviation_; }

  const flags_t& getHotPixels() const { return hotPixels_; }
  flags_t& getHotPixels() { return hotPixels_; }

  void setBadFrame(bool v) { badFrame_ = v; }
  bool BadFrame() { return badFrame_; }

  double rawFrameMedian() const;

  void setTrimSize(int v) { trimSize_ = v; }
  int TrimSize() { return trimSize_; }

protected:
  bool isMaxPixel(int ix, int iy, int size) const;
  bool includeHotPixel(int ix, int iy, int size) const;

private:
  const int num_pixels_x_ = 1;
  const int num_pixels_y_ = 1;

  int eventSize_ = 1;
  int trimSize_ = 0;

  image_t rawFrame_;
  image_t frame_;
  image_t pedestals_;

  image_t weight_;
  image_t sum_;
  image_t sum2_;
  image_t deviation_;
  std::vector<char> buf_;

  double eventThreshold_ = 0.0;
  double splitThreshold_ = 0.0;
  double hotPixelThreshold_ = 0.0;

  flags_t hotPixels_;
  bool badFrame_ = false;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_FrameData_H */
