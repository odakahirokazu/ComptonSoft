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

#include <anlnext/BasicModule.hh>
#include <memory>

namespace comptonsoft {

class FrameData;


/**
 * ConstructFrame
 *
 * @author Hirokazu Odaka
 * @date 2019-06-05
 */
class ConstructFrame : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(ConstructFrame, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  ConstructFrame();
  
protected:
  ConstructFrame(const ConstructFrame&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;

  void setFrameID(int v) { frame_id_ = v; }
  int FrameID() const { return frame_id_; }

  const comptonsoft::FrameData& getFrame() const
  { return *frame_; }

  comptonsoft::FrameData& getFrame()
  { return *frame_; }

protected:
  virtual FrameData* createFrameData();
  int NumPixelsX() const { return num_pixel_x_; }
  int NumPixelsY() const { return num_pixel_y_; }

private:
  int num_pixel_x_ = 1;
  int num_pixel_y_ = 1;

  int frame_id_ = 0;
  std::unique_ptr<comptonsoft::FrameData> frame_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ConstructFrame_H */
