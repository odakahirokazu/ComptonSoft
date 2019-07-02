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

namespace comptonsoft
{

using image_t = boost::multi_array<double, 2>;

/**
 * A class of an X-ray event measured with a pixel detector.
 *
 * @author Hirokazu Odaka
 * @date 2019-05-22
 */
class XrayEvent
{
public:
  explicit XrayEvent(int size);
  virtual ~XrayEvent();

  XrayEvent(const XrayEvent& r) = default;
  XrayEvent(XrayEvent&& r) = default;
  XrayEvent& operator=(const XrayEvent& r) = default;
  XrayEvent& operator=(XrayEvent&& r) = default;

  int EventSize() const { return size_; }

  void setSplitThreshold(double v) { splitThreshold_ = v; }
  double SplitThreshold() const { return splitThreshold_; }

  void setFrameID(int v) { frameID_ = v; }
  int FrameID() const { return frameID_; }

  void setTime(double v) { time_ = v; }
  double Time() const { return time_; }

  virtual void copyFrom(const image_t& image, int ix, int iy);
  virtual void reduce();

  const image_t& Data() const { return data_; }
  int X() const { return  ix_;  }
  int Y() const { return  iy_;  }
  
  double Value() const { return value_; }
  int Weight() const { return weight_; }
  int Rank() const { return rank_;}
  double Angle() const { return angle_; }
  double CenterPH() const { return centerPH_;}
  int Grade() const { return grade_; }

protected:
  image_t& Data() { return data_; }

  void setValue(double v) { value_ = v; }
  void setWeight(int v) { weight_ = v; }
  void setRank(int v) { rank_ = v;}
  void setAngle(double v) { angle_ = v; }
  void setCenterPH(double v) { centerPH_ = v; }
  void setGrade(int v) { grade_ = v; }
  void setData(int i, int j, double v) { data_[i][j]=v; }

  virtual double calculateEventAngle() const;

private:
  const int size_ = 1;
  double splitThreshold_ = 0.0;

  int frameID_ = 0;
  double time_ = 0.0;
  int ix_ = 0;
  int iy_ = 0;
  image_t data_;
  
  double value_ = 0.0;
  int weight_ = 0;
  int rank_ = 0;
  double angle_ = 0.0;
  double centerPH_ = 0.0;
  int grade_ = -1;
};

using XrayEvent_sptr = std::shared_ptr<XrayEvent>;
using const_XrayEvent_sptr = std::shared_ptr<const XrayEvent>;
using XrayEventContainer = std::list<XrayEvent_sptr>;
using XrayEventCIter = XrayEventContainer::const_iterator;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_XrayEvent_H */
