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

#ifndef COMPTONSOFT_SXIFrameData_H
#define COMPTONSOFT_SXIFrameData_H 1

#include <string>
#include <vector>
#include <list>
#include <cstdint>


#include "FrameData.hh"
#include "SXIXrayEvent.hh"
namespace comptonsoft
{

/**
 * A class of an SXI CCD frame.
 *
 * @author Tsubasa Tamba
 * @date 2019-06-03
 * @date 2019-10-08 | Hirokazu Odaka | delete the assignment operators
 */
class SXIFrameData: public FrameData
{
public:
  SXIFrameData(int nx, int ny);
  virtual ~SXIFrameData();
  
  SXIFrameData(const SXIFrameData& r) = default;
  SXIFrameData(SXIFrameData&& r) = default;

  std::vector<XrayEvent_sptr> extractEvents() override;
  double SurroundThreshold() const { return surroundThreshold_;}
  int NpixSurroundThreshold() const { return npixSurroundThreshold_;}
  double OuterSplitThreshold() const { return outerSplitThreshold_; }

  void setOuter(double threshold)
  {
    outerSplitThreshold_ = threshold;
  }
  
  void setSurroundDiscri(double surround, int npix)
  {
    surroundThreshold_ = surround;
    npixSurroundThreshold_ = npix;
  }

protected:
  bool surroundDiscri(image_t& frame, int ix, int iy, int size, double surroundThreshold, int npixSurroundThreshold);

private:
  SXIFrameData& operator=(const SXIFrameData& r) = delete;
  SXIFrameData& operator=(SXIFrameData&& r) = delete;

private:
  double outerSplitThreshold_ = 0.0;
  double surroundThreshold_ = 0.0;
  int npixSurroundThreshold_ = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SXIFrameData_H */
