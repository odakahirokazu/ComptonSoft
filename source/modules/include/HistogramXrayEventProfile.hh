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
 * HistogramXrayEventProfile
 *
 * @author Taihei Watanabe
 * @date 2020-10-26
 */

#ifndef COMPTONSOFT_HistogramXrayEventProfile_H
#define COMPTONSOFT_HistogramXrayEventProfile_H 1

#include "VCSModule.hh"

class TH1;

namespace comptonsoft {

class XrayEventCollection;

class HistogramXrayEventProfile : public VCSModule
{
  DEFINE_ANL_MODULE(HistogramXrayEventProfile, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  HistogramXrayEventProfile();

protected:
  HistogramXrayEventProfile(const HistogramXrayEventProfile&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames) override;

private:
  int numBins_ = 1;
  double min_ = 0.0;
  double max_ = 1.0;
  int axis_ = 0;
  std::string collectionModule_;
  std::string outputName_;

  XrayEventCollection* collection_ = nullptr;
  TH1* histogram_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HistogramXrayEventProfile_H */
