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

/**
 * @file TimeProfile.hh
 * @brief header file of class TimeProfile
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#ifndef COMPTONSOFT_TimeProfile_H
#define COMPTONSOFT_TimeProfile_H 1

#include <vector>
#include <string>

namespace comptonsoft
{

struct TimeInterval
{
  double time1;
  double time2;
  double rate;
};

class TimeProfile
{
public:
  TimeProfile() = default;
  ~TimeProfile();
  TimeProfile(const TimeProfile&) = default;
  TimeProfile(TimeProfile&&) = default;
  TimeProfile& operator=(const TimeProfile&) = default;
  TimeProfile& operator=(TimeProfile&&) = default;

  void clear()
  {
    time1_.clear();
    time2_.clear();
    rate_.clear();
  }

  void push(double t1, double t2, double rate)
  {
    time1_.push_back(t1);
    time2_.push_back(t2);
    rate_.push_back(rate);
  }
  
  bool readFile(const std::string& filename);

  std::size_t NumberOfIntervals() const { return time1_.size(); }
  TimeInterval getInterval(std::size_t i) const
  { return TimeInterval{time1_[i], time2_[i], rate_[i]}; }
  
private:
  std::vector<double> time1_;
  std::vector<double> time2_;
  std::vector<double> rate_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_TimeProfile_H */
