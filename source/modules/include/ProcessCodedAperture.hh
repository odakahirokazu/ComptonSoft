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
 * update 2020-10-27
 */

#ifndef COMPTONSOFT_ProcessCodedAperture_H
#define COMPTONSOFT_ProcessCodedAperture_H 1

#include "VCSModule.hh"

#include <memory>
#include "XrayEvent.hh"
#include "VCodedAperture.hh"
#include "ExtractXrayEventImage.hh"
#include "ExtractXrayEventImageFromQuickLookDB.hh"

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
  anlnext::ANLStatus mod_end_run() override;

  int ApertureNumX() const { return apertureNumX_; }
  int ApertureNumY() const { return apertureNumY_; }
  int SkyNumX() const { return skyNumX_; }
  int SkyNumY() const { return skyNumY_; }
  double DetectorElementSize() const { return detectorElementSize_; }
  double ApertureElementSize() const { return apertureElementSize_; }
  double SkyFovX() const { return skyFovX_; }
  double SkyFovY() const { return skyFovY_; }
  double DetectorToApertureDistance() const { return detectorToApertureDistance_; }
  double DetectorRollAngle() const { return detectorRollAngle_; }
  double ApertureRollAngle() const { return apertureRollAngle_; }
  double ApertureOffsetX() const { return apertureOffsetX_; }
  double ApertureOffsetY() const { return apertureOffsetY_; }
  double SkyOffsetX() const { return skyOffsetX_; }
  double SkyOffsetY() const { return skyOffsetY_; }
  int NumDecodingIterations() const { return numDecodingIterations_; }
  int DecodingMode() const { return decodingMode_; }

  void drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames) override;

protected:
  virtual std::unique_ptr<VCodedAperture> createCodedAperture();
  virtual void decode();
  void fillHistogram();

private:
  int apertureNumX_ = 1;
  int apertureNumY_ = 1;
  int skyNumX_ = 1;
  int skyNumY_ = 1;
  double detectorElementSize_ = 1.0;
  double apertureElementSize_ = 1.0;
  double skyFovX_ = 0.0;
  double skyFovY_ = 0.0;
  double detectorToApertureDistance_ = 1.0;
  double detectorRollAngle_ = 0.0;
  double apertureRollAngle_ = 0.0;
  double apertureOffsetX_ = 0.0;
  double apertureOffsetY_ = 0.0;
  double skyOffsetX_ = 0.0;
  double skyOffsetY_ = 0.0;
  int numDecodingIterations_ = 1;
  int decodingMode_ = 1;
  
  std::string patternFile_;
  std::string imageOwnerModule_;
  std::string outputName_;
  std::string outputAngleUnit_;

  std::shared_ptr<image_t> decodedImage_;
  VCSModule* imageOwner_ = nullptr;
  std::unique_ptr<VCodedAperture> codedAperture_;
  TH2* histogram_ = nullptr;
  bool finalHistogramReady_ = false;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ProcessCodedAperture_H */
