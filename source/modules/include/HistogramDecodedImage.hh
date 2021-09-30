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
 * HistogramDecodedImage
 *
 * @author Taihei Watanabe
 * @date 2021-09-28
 */

#ifndef COMPTONSOFT_HistogramDecodedImage_H
#define COMPTONSOFT_HistogramDecodedImage_H 1

#include "VCSModule.hh"
#include "ProcessCodedAperture.hh"

class TH2;

namespace comptonsoft {

class HistogramDecodedImage : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramDecodedImage, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  HistogramDecodedImage();
  
protected:
  HistogramDecodedImage(const HistogramDecodedImage&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void sumImages();

  void drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames) override;

private:
  std::vector<std::string> decodingModuleNames_;
  std::string outputName_;
  
  std::vector<ProcessCodedAperture*> modules_;
  TH2* histogram_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramDecodedImage_H */
