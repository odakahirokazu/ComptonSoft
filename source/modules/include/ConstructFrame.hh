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

#ifndef COMPTONSOFT_ConstructFrame_H
#define COMPTONSOFT_ConstructFrame_H 1

#include "ConstructDetector.hh"
#include "DetectorSystem.hh"

namespace comptonsoft {

class FrameData;


/**
 * ConstructFrame
 *
 * @author Hirokazu Odaka
 * @date 2019-06-05
 * @date 2020-04-01 | v2.0 | based on ConstructDetector
 */
class ConstructFrame : public ConstructDetector
{
  DEFINE_ANL_MODULE(ConstructFrame, 2.0);
  // ENABLE_PARALLEL_RUN();
public:
  ConstructFrame();
  
protected:
  ConstructFrame(const ConstructFrame&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;

protected:
  int NumPixelsX() const { return num_pixels_x_; }
  int NumPixelsY() const { return num_pixels_y_; }

  virtual std::unique_ptr<FrameData> createFrameData();

private:
  int num_pixels_x_ = 1;
  int num_pixels_y_ = 1;
  std::vector<int> ids_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ConstructFrame_H */
