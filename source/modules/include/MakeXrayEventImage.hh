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
 * MakeXrayEventImage
 *
 * @author Tsubasa Tamba
 * @date 2019-11-01
 */

#ifndef COMPTONSOFT_MakeXrayEventImage_H
#define COMPTONSOFT_MakeXrayEventImage_H 1

#include <anlnext/BasicModule.hh>
#include <memory>
#include "XrayEvent.hh"
#include "VCSModule.hh"
class TH2;

namespace comptonsoft {

class XrayEventCollection;
class XrayEventTreeIO;

class MakeXrayEventImage : public VCSModule
{
  DEFINE_ANL_MODULE(MakeXrayEventImage, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  MakeXrayEventImage();
  
protected:
  MakeXrayEventImage(const MakeXrayEventImage&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  int NumPixelX() const { return numPixelX_; }
  int NumPixelY() const { return numPixelY_; }
  int OffsetX() const { return offsetX_; }
  int OffsetY() const { return offsetY_; }
  double RotationCenterX() const { return rotationCenterX_; }
  double RotationCenterY() const { return rotationCenterY_; }
  double RotationAngle() const { return rotationAngle_; }
  std::shared_ptr<image_t> Image() { return image_; }
  std::shared_ptr<image_t> TotalImage() { return totalImage_; }

  void drawOutputFiles(TCanvas* c1, std::vector<std::string>* filenames) override;

protected:
  void resetImage(std::shared_ptr<image_t> image);
  void rotateImage(std::shared_ptr<image_t> image);
  void fillHistogram();

private:
  std::string collectionModule_;
  int numPixelX_ = 1;
  int numPixelY_ = 1;
  int offsetX_ = 0;
  int offsetY_ = 0;
  double rotationAngle_ = 0.0;
  double rotationCenterX_ = 0.0;
  double rotationCenterY_ = 0.0;

  std::shared_ptr<image_t> image_;
  std::shared_ptr<image_t> totalImage_;

  XrayEventCollection* collection_ = nullptr;
  TH2* totalHistogram_ = nullptr;
  std::string outputFile_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_MakeXrayEventImage_H */
