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

#ifndef COMPTONSOFT_SXIXrayEvent_H
#define COMPTONSOFT_SXIXrayEvent_H 1

#include <list>
#include <memory>
#include <boost/multi_array.hpp>
#include <vector>

#include "XrayEvent.hh"

namespace comptonsoft
{

//using image_t = boost::multi_array<double, 2>;

/**
 * A class of an X-ray event measured with an SXI CCD.
 *
 * @author Tsubasa Tamba
 * @date 2019-06-03
 */
class SXIXrayEvent: public XrayEvent
{
public:
  explicit SXIXrayEvent(int size);
  virtual ~SXIXrayEvent();

  SXIXrayEvent(const SXIXrayEvent& r) = default;
  SXIXrayEvent(SXIXrayEvent&& r) = default;
  SXIXrayEvent& operator=(const SXIXrayEvent& r) = default;
  SXIXrayEvent& operator=(SXIXrayEvent&& r) = default;

  void reduce() override;
  void setOuterSplitThreshold(double v) { outerSplitThreshold_ = v; }
  void determineAscaGrade();
  double OuterSplitThreshold() const { return outerSplitThreshold_; }
  int makeOuterMask (int innerGrade);
  void classifyGrade();
  double calculateSxiSumPH();
  void calculateRank();
  void calculateWeightAndModifyData();

private:
  double outerSplitThreshold_ = 0.0;
  int sxiInnerGrade_ = 0;
  int sxiOuterGrade_ = 0;
  int sxiTotalGrade_ = 0;
  int outerMask_ = 0;
  int sxiOuterMaskedGrade_ = 0;
  std::vector <double> dataVector_;
};

using SXIXrayEvent_sptr = std::shared_ptr<SXIXrayEvent>;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SXIXrayEvent_H */
