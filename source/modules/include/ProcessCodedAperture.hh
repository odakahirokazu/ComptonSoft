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
 * ProcessCodedAperture
 *
 * @author Tsubasa Tamba
 * @date 2019-11-01
 */

#ifndef COMPTONSOFT_ProcessCodedAperture_H
#define COMPTONSOFT_ProcessCodedAperture_H 1

#include <anlnext/BasicModule.hh>
#include "XrayEvent.hh"
#include "CodedAperture.hh"
#include "MakeXrayEventImage.hh"
#include "VCSModule.hh"

class TH2;

namespace comptonsoft {

class ProcessCodedAperture : public VCSModule
{
  DEFINE_ANL_MODULE(ProcessCodedAperture, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  ProcessCodedAperture();
  
protected:
  ProcessCodedAperture(const ProcessCodedAperture&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  int NumEncodedImageX() { return numEncodedImageX_; }
  int NumEncodedImageY() { return numEncodedImageY_; }
  int NumDecodedImageX() { return numDecodedImageX_; }
  int NumDecodedImageY() { return numDecodedImageY_; }
  int NumMaskX() { return numMaskX_; }
  int NumMaskY() { return numMaskY_; }
  double DetectorElementSize() { return detectorElementSize_; }
  double MaskElementSize() { return maskElementSize_; }
  double MaskToDetectorDistance() { return maskToDetectorDistance_; }
  double SourceToMaskDistance() { return sourceToMaskDistance_; }

  void drawOutputFiles(TCanvas* c1, std::vector<std::string>* filenames) override;

protected:
  virtual CodedAperture* createCodedAperture();
  void fillHistogram();

private:
  int numEncodedImageX_ = 1;
  int numEncodedImageY_ = 1;
  int numDecodedImageX_ = 1;
  int numDecodedImageY_ = 1;
  int numMaskX_ = 1;
  int numMaskY_ = 1;

  double detectorElementSize_ = 1.0;
  double maskElementSize_ = 1.0;
  double maskToDetectorDistance_ = 1.0;
  double sourceToMaskDistance_ = 1.0;

  std::string patternFile_;
  image_t pattern_;
  image_t accumulatedDecodedImage_;
  image_t totalDecodedImage_;
  bool analyzeEveryTime_ = false;
  MakeXrayEventImage* image_owner_ = nullptr;
  std::string imageOwnerModule_ = "MakeXrayEventImage";
  std::unique_ptr<comptonsoft::CodedAperture> coded_aperture_;
  TH2* totalHistogram_ = nullptr;
  std::string outputFile_ = "DecodedImage.png";

};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ProcessCodedAperture_H */
