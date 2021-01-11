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
 * @file RateData.hh
 * @brief header file of class RateData
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#ifndef COMPTONSOFT_RateData_H
#define COMPTONSOFT_RateData_H 1

#include <vector>
#include <string>
#include "IsotopeInfo.hh"

namespace comptonsoft
{

using RateVector = std::vector<IsotopeInfo>;


class RateData
{
public:
  RateData() = default;
  ~RateData();
  RateData(const RateData&) = default;
  RateData(RateData&&) = default;
  RateData& operator=(const RateData&) = default;
  RateData& operator=(RateData&&) = default;

  bool readFile(const std::string& filename);
  bool writeFile(const std::string& filename);

  void setCountThreshold(double v) { count_threshold_ = v; }
  double CountThreshold() const { return count_threshold_; }

  std::size_t NumberOfVolumes() const
  { return data_.size(); }

  std::string getVolumeName(std::size_t i) const
  { return data_[i].first; }

  RateVector getRateVector(std::size_t i) const
  { return data_[i].second; }

  void pushData(const std::string volumeName, const RateVector& rateVector)
  { data_.push_back(std::make_pair(volumeName, rateVector)); }
  
private:
  std::vector<std::pair<std::string, RateVector>> data_;
  double count_threshold_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RateData_H */
