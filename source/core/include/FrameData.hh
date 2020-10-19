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
#include <memory>
#include "XrayEvent.hh"
#include "OutlierStore.hh"

namespace comptonsoft
{

class VGainFunction;


/**
 * A class of a frame measured with a frame-readout pixel detector.
 *
 * @author Hirokazu Odaka
 * @date 2019-05-22
 * @date 2019-10-08 | delete the assignment operators
 * @date 2020-03-31 | add gain functions
 * @date 2020-05-28 | Kosuke Hatauchi & Hirokazu odaka | revise pedestal statistics
 * @date 2020-07-31 | Hirokazu odaka | byte order of raw data
 * @date 2020-08-01 | Hirokazu odaka | pixel shift at odd rows
 */
class FrameData
{
public:
  using flags_t = boost::multi_array<int, 2>;
  enum class CornerID { BottomLeft=0, UpperLeft=1, BottomRight=2, UpperRight=3 };

public:
  FrameData(int nx, int ny);
  virtual ~FrameData();
  
  FrameData(const FrameData& r) = default;
  FrameData(FrameData&& r) = default;

  int NumPixelsX() const { return num_pixels_x_; }
  int NumPixelsY() const { return num_pixels_y_; }

  void setByteOrder(bool v) { byte_order_ = v; }
  bool ByteOrder() const { return byte_order_; }

  void setOddRowPixelShift(int v) { odd_row_pixel_shift_ = v; }
  int OddRowPixelShift() const { return odd_row_pixel_shift_; }

  void setStartPosition(CornerID id) { startPosition_ = id; }
  CornerID StartPosition() { return startPosition_; }

  void setReadDirectionX(bool v) { readDirectionX_ = v; }
  bool ReadDirectionX() { return readDirectionX_; }

  void setFrameID(int v) { frameID_ = v; }
  int FrameID() const { return frameID_; }

  void setEventSize(int v) { eventSize_ = v; }
  int EventSize() const { return eventSize_; }

  void setEventThreshold(double v) { eventThreshold_ = v; }
  void setSplitThreshold(double v) { splitThreshold_ = v; }
  double EventThreshold() const { return eventThreshold_; }
  double SplitThreshold() const { return splitThreshold_; }

  void setStatisticsExclusionNumbers(int num_low, int num_high);
  
  void resetRawFrame();
  virtual bool load(const std::string& filename);
  void stack();

  void setPedestals(double v);
  virtual void calculateStatistics();
  virtual void subtractPedestals();
  virtual std::vector<XrayEvent_sptr> extractEvents();

  void selectGoodPixels(double mean_min, double mean_max,
                        double sigma_min, double sigma_max);

  void setRawFrame(const image_t& v) { rawFrame_ = v; }
  void setFrame(const image_t& v) { frame_ = v; }
  const image_t& getRawFrame() const { return rawFrame_; }
  image_t& getRawFrame() { return rawFrame_; }
  const image_t& getFrame() const { return frame_; }
  image_t& getFrame() { return frame_; }

  void setPedestal(int ix, int iy, double v) { pedestals_[ix][iy] = v; }
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

  void setDisabledPixel(int ix, int iy, int v) { disabledPixels_[ix][iy] = v; }
  const flags_t& getDisabledPixels() const { return disabledPixels_; }
  flags_t& getDisabledPixels() { return disabledPixels_; }
  bool isDisabledPixel(int ix, int iy) const  { return !isNotDisabledPixel(ix, iy); }
  bool isNotDisabledPixel(int ix, int iy) const { return disabledPixels_[ix][iy]==0; }

  void setBadFrame(bool v) { badFrame_ = v; }
  bool BadFrame() { return badFrame_; }

  double RawFrameMedian() const;

  void setTrimSize(int v) { trimSize_ = v; }
  int TrimSize() { return trimSize_; }

  void setCommonGainFunction(const std::shared_ptr<VGainFunction>& f);
  void setGainFunction(int ix, int iy, const std::shared_ptr<VGainFunction>& f);
  void resetGainFunctions();

  void correctGains();

protected:
  bool isMaxPixel(int ix, int iy, int size) const;
  bool includeDisabledPixel(int ix, int iy, int size) const;

  VGainFunction* getGainFunction(int ix, int iy) const;
  double correctGain(int ix, int iy, double pha) const;

private:
  FrameData& operator=(const FrameData& r) = delete;
  FrameData& operator=(FrameData&& r) = delete;

private:
  const int num_pixels_x_ = 1;
  const int num_pixels_y_ = 1;

  bool byte_order_ = true; // big/little endian for true/false
  int odd_row_pixel_shift_ = 0;
  CornerID startPosition_;
  bool readDirectionX_ = false;

  int frameID_ = 0;
  int eventSize_ = 1;
  int trimSize_ = 0;
  bool badFrame_ = false;

  double eventThreshold_ = 0.0;
  double splitThreshold_ = 0.0;

  std::vector<char> buf_;

  image_t rawFrame_;
  image_t frame_;
  image_t pedestals_;

  image_t weight_;
  image_t sum_;
  image_t sum2_;
  image_t deviation_;
  flags_t disabledPixels_;
  boost::multi_array<std::unique_ptr<OutlierStore>, 2> pixelValuesToExclude_;

  using gain_func_array_t = boost::multi_array<std::shared_ptr<VGainFunction>, 2>;
  bool shareGainFunction_ = true;
  std::shared_ptr<VGainFunction> commonGainFunction_;
  std::shared_ptr<gain_func_array_t> gainFunctions_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_FrameData_H */
