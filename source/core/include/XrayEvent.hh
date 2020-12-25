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

#ifndef COMPTONSOFT_XrayEvent_H
#define COMPTONSOFT_XrayEvent_H 1

#include <list>
#include <memory>
#include <boost/multi_array.hpp>
#include "CSTypes.hh"

namespace comptonsoft
{

using image_t = boost::multi_array<double, 2>;

/**
 * A class of an X-ray event measured with a pixel detector.
 *
 * @author Hirokazu Odaka
 * @date 2019-05-22
 * @date 2019-10-08 | delete the assignment operators
 * @date 2020-04-02 | add propeties
 */
class XrayEvent
{
public:
  explicit XrayEvent(int size);
  virtual ~XrayEvent();

  XrayEvent(const XrayEvent& r) = default;
  XrayEvent(XrayEvent&& r) = default;

  int EventSize() const { return size_; }

  virtual void copyFrom(const image_t& image, int ix, int iy);
  virtual void reduce();

  void setSplitThreshold(double v) { splitThreshold_ = v; }
  double SplitThreshold() const { return splitThreshold_; }

  void setFrameID(int v) { frameID_ = v; }
  int FrameID() const { return frameID_; }

  void setTime(double v) { time_ = v; }
  double Time() const { return time_; }

  void setDetectorID(int v) { detectorID_ = v; }
  int DetectorID() const { return detectorID_; }

  void setPixelX(int v) { ix_ = v; }
  void setPixelY(int v) { iy_ = v; }
  int PixelX() const { return  ix_;  }
  int PixelY() const { return  iy_;  }

  void setData(int i, int j, double v) { data_[i][j] = v; }
  image_t& Data() { return data_; }
  const image_t& Data() const { return data_; }
  
  void setSumPH(double v) { sumPH_ = v; }
  void setCenterPH(double v) { centerPH_ = v; }
  void setWeight(int v) { weight_ = v; }
  void setRank(int v) { rank_ = v;}
  void setAngle(double v) { angle_ = v; }
  void setGrade(int v) { grade_ = v; }

  double SumPH() const { return sumPH_; }
  int Weight() const { return weight_; }
  int Rank() const { return rank_;}
  double Angle() const { return angle_; }
  double CenterPH() const { return centerPH_;}
  int Grade() const { return grade_; }

  void setEnergy(double v) { energy_ = v; }
  void setPositionX(double v) { posx_ = v; }
  void setPositionY(double v) { posy_ = v; }
  void setPositionZ(double v) { posz_ = v; }
  void setPosition(double x, double y, double z)
  { posx_ = x; posy_ = y; posz_ = z; }
  void setPosition(const vector3_t v)
  { posx_ = v.x(); posy_ = v.y(); posz_ = v.z(); }

  double Energy() const { return energy_; }
  double PositionX() const { return posx_; }
  double PositionY() const { return posy_; }
  double PositionZ() const { return posz_; }
  vector3_t Position() const { return vector3_t(posx_, posy_, posz_); }

protected:
  virtual double calculateEventAngle() const;

private:
  XrayEvent& operator=(const XrayEvent& r) = delete;
  XrayEvent& operator=(XrayEvent&& r) = delete;

private:
  const int size_ = 1;
  double splitThreshold_ = 0.0;

  int frameID_ = 0;
  double time_ = 0.0;
  int detectorID_ = 0;
  int ix_ = 0;
  int iy_ = 0;
  image_t data_;
  
  double sumPH_ = 0.0;
  double centerPH_ = 0.0;
  int weight_ = 0;
  int rank_ = 0;
  double angle_ = 0.0;
  int grade_ = -1;

  double energy_ = 0.0;
  double posx_ = 0.0;
  double posy_ = 0.0;
  double posz_ = 0.0;
};

template <typename T>
bool filterXrayEvent(T (XrayEvent::*getter)() const,
                    const XrayEvent& event,
                    T min, T max)
{
  return min <= (event.*getter)() && (event.*getter)() <= max;
}

using XrayEvent_sptr = std::shared_ptr<XrayEvent>;
using const_XrayEvent_sptr = std::shared_ptr<const XrayEvent>;
using XrayEventContainer = std::list<XrayEvent_sptr>;
using XrayEventCIter = XrayEventContainer::const_iterator;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_XrayEvent_H */
