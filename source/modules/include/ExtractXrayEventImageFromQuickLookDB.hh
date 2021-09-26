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


#ifndef COMPTONSOFT_ExtractXrayEventImageFromQuickLookDB_H
#define COMPTONSOFT_ExtractXrayEventImageFromQuickLookDB_H 1

#include "VCSModule.hh"
#include <memory>
#include <random>
#include "ExtractXrayEventImage.hh"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>


class TH2;

namespace comptonsoft {

class XrayEventCollection;

class ExtractXrayEventImageFromQuickLookDB : public VCSModule
{
  DEFINE_ANL_MODULE(ExtractXrayEventImageFromQuickLookDB, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  ExtractXrayEventImageFromQuickLookDB();
  
protected:
  ExtractXrayEventImageFromQuickLookDB(const ExtractXrayEventImageFromQuickLookDB&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  std::shared_ptr<image_t> Image() override { return image_; }

  void drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames) override;

protected:
  double sampleRandomNumber();
  void fillHistogram();

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
  std::string collectionName_;
  std::string dbName_;
  std::string analysisId_;
  std::string outputName_;

  double axx_ = 1.0;
  double axy_ = 0.0;
  double ayx_ = 0.0;
  double ayy_ = 1.0;
  std::default_random_engine randomGenerator_;
  std::uniform_real_distribution<double> distribution_;

  // mongocxx::database db_;

  XrayEventCollection* collection_ = nullptr;
  std::shared_ptr<image_t> image_;
  TH2* histogram_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ExtractXrayEventImageFromQuickLookDB_H */
