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
 * HistogramRawFrameImage
 *
 * @author Tsubasa Tamba
 * @date 2020-11-02
 * @date 2023-09-13 | H.Odaka | draw-each-frame mode
 */

#ifndef COMPTONSOFT_HistogramRawFrameImage_H
#define COMPTONSOFT_HistogramRawFrameImage_H 1

#include "VCSModule.hh"
#include <memory>
#include "FrameData.hh"
#include "XrayEvent.hh"

class TH2;

namespace comptonsoft {

class HistogramRawFrameImage : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramRawFrameImage, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  HistogramRawFrameImage();

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  void drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames) override;

protected:
  void fillHistogram();

private:
  int detectorID_ = 0;
  std::string outputName_;
  int rebinX_ = 1;
  int rebinY_ = 1;
  bool update_each_frame_ = false;

  FrameData* frame_ = nullptr;
  TH2* histogram_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramRawFrameImage_H */
