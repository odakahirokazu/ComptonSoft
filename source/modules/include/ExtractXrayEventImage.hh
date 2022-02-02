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
 * ExtractXrayEventImage
 *
 * @author Tsubasa Tamba
 * @date 2019-11-01
 * @date 2019-11-21 | H. Odaka | modify image definition
 */

#ifndef COMPTONSOFT_ExtractXrayEventImage_H
#define COMPTONSOFT_ExtractXrayEventImage_H 1

#include "VCSModule.hh"
#include <memory>
#include <random>
#include "XrayEvent.hh"

class TH2;

namespace comptonsoft {

class XrayEventCollection;

class ExtractXrayEventImage : public VCSModule
{
  DEFINE_ANL_MODULE(ExtractXrayEventImage, 1.1);
  // ENABLE_PARALLEL_RUN();
public:
  ExtractXrayEventImage();
  
protected:
  ExtractXrayEventImage(const ExtractXrayEventImage&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  std::shared_ptr<image_t> Image() { return image_; }
  std::shared_ptr<image_t> RecentImage() { return recentImage_; }

  void drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames) override;

protected:
  double sampleRandomNumber();
  void fillHistogram();

  XrayEventCollection* getEventCollection() { return collection_; }

private:
  int numX_ = 1;
  int numY_ = 1;
  double imageCenterX_ = 0.0;
  double imageCenterY_ = 0.0;
  double rotationAngle_ = 0.0;
  double scale_ = 1.0;
  double newOriginX_ = 0.0;
  double newOriginY_ = 0.0;
  int rebinX_ = 1;
  int rebinY_ = 1;
  bool randomSampling_ = false;
  std::string collectionModule_;
  std::string outputName_;

  double axx_ = 1.0;
  double axy_ = 0.0;
  double ayx_ = 0.0;
  double ayy_ = 1.0;
  std::default_random_engine randomGenerator_;
  std::uniform_real_distribution<double> distribution_;

  XrayEventCollection* collection_ = nullptr;
  std::shared_ptr<image_t> image_;
  std::shared_ptr<image_t> recentImage_;
  TH2* histogram_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ExtractXrayEventImage_H */
