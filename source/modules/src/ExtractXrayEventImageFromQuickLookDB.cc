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

#include "ExtractXrayEventImageFromQuickLookDB.hh"

#include <cmath>
#include "AstroUnits.hh"
#include "TH2.h"
#include "TStyle.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace {

void reset_image(std::shared_ptr<comptonsoft::image_t> image)
{
  const int nx = (*image).shape()[0];
  const int ny = (*image).shape()[1];
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      (*image)[ix][iy] = 0.0;
    }
  }
}

} /* anonymous namespace */

namespace comptonsoft {

ExtractXrayEventImageFromQuickLookDB::ExtractXrayEventImageFromQuickLookDB()
  : outputName_("image"),
    distribution_(0.0, 1.0)
{
}

ANLStatus ExtractXrayEventImageFromQuickLookDB::mod_define()
{
  define_parameter("num_x", &mod_class::numX_);
  define_parameter("num_y", &mod_class::numY_);
  define_parameter("image_center_x", &mod_class::imageCenterX_);
  define_parameter("image_center_y", &mod_class::imageCenterY_);
  define_parameter("rotation_angle", &mod_class::rotationAngle_, unit::degree, "degree");
  define_parameter("scale", &mod_class::scale_);
  define_parameter("new_origin_x", &mod_class::newOriginX_);
  define_parameter("new_origin_y", &mod_class::newOriginY_);
  define_parameter("rebin_x", &mod_class::rebinX_);
  define_parameter("rebin_y", &mod_class::rebinY_);
  define_parameter("random_sampling", &mod_class::randomSampling_);
  define_parameter("db_name", &mod_class::dbName_);
  define_parameter("collection_name", &mod_class::collectionName_);
  define_parameter("analysis_id", &mod_class::analysisId_);
  define_parameter("output_name", &mod_class::outputName_);
  
  return AS_OK;
}

ANLStatus ExtractXrayEventImageFromQuickLookDB::mod_initialize()
{
  image_.reset(new image_t(boost::extents[numX_][numY_]));
  reset_image(image_);

  const double cosTheta = std::cos(rotationAngle_);
  const double sinTheta = std::sin(rotationAngle_);
  axx_ =  scale_*cosTheta;
  axy_ = -scale_*sinTheta;
  ayx_ =  scale_*sinTheta;
  ayy_ =  scale_*cosTheta;

  ANLStatus status = VCSModule::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }
  mkdir();
  const std::string name = "image";
  const std::string title = "Image";
  const int nx = numX_/rebinX_;
  const int ny = numY_/rebinY_;
  histogram_ = new TH2D(name.c_str(), title.c_str(),
                        nx, 0.0, static_cast<double>(numX_),
                        ny, 0.0, static_cast<double>(numY_));

  return AS_OK;
}

ANLStatus ExtractXrayEventImageFromQuickLookDB::mod_analyze()
{
  const int Nx = image_->shape()[0];
  const int Ny = image_->shape()[1];
  const double Cx = imageCenterX_;
  const double Cy = imageCenterY_;
  const double Axx = axx_;
  const double Axy = axy_;
  const double Ayx = ayx_;
  const double Ayy = ayy_;
  const double Ox = newOriginX_;
  const double Oy = newOriginY_;

  reset_image(image_);

  {
    using namespace mongocxx;
    using namespace bsoncxx::builder::basic;

    mongocxx::pipeline p{};
    p.match(make_document(kvp("loop_id", analysisId_)));
    p.group(make_document(
      kvp("_id", make_document(kvp("ix", "$ix"), kvp("iy", "$iy"))),
      kvp("count", make_document(kvp("$sum", 1)))));

    const mongocxx::client conn{mongocxx::uri{}};
    auto db_ = conn[dbName_];
    
    auto cursor = db_[collectionName_].aggregate(p, mongocxx::options::aggregate{});

    for (auto&& elem : cursor) {
      double x0 = elem["_id"]["ix"].get_int32();
      double y0 = elem["_id"]["iy"].get_int32();
      double count = elem["count"].get_int32();

      if (randomSampling_) {
        x0 += sampleRandomNumber();
        y0 += sampleRandomNumber();
      }
      x0 -= Cx;
      y0 -= Cy;
      const double x1 = Axx*x0 + Axy*y0 + Cx - Ox;
      const double y1 = Ayx*x0 + Ayy*y0 + Cy - Oy;
      const int ix1 = static_cast<int>(std::floor(x1));
      const int iy1 = static_cast<int>(std::floor(y1));
      if (0<=ix1 && ix1<Nx && 0<=iy1 && iy1<Ny) {
        (*image_)[ix1][iy1] += count;
      }
    }
  }

  return AS_OK;
}

ANLStatus ExtractXrayEventImageFromQuickLookDB::mod_end_run()
{
  fillHistogram();

  return AS_OK;
}

double ExtractXrayEventImageFromQuickLookDB::sampleRandomNumber()
{
  return distribution_(randomGenerator_);
}

void ExtractXrayEventImageFromQuickLookDB::fillHistogram()
{
  const image_t& image = *image_;
  TH2& h = *histogram_;

  const int Nx = image.shape()[0];
  const int Ny = image.shape()[1];
  for (int ix=0; ix<Nx; ix++) {
    for (int iy=0; iy<Ny; iy++) {
      const double v = image[ix][iy];
      h.Fill(ix, iy, v);
    }
  }
}

void ExtractXrayEventImageFromQuickLookDB::drawCanvas(TCanvas* canvas, std::vector<std::string>* filenames)
{
  fillHistogram();

  const std::string outputFile = outputName_+".png";
  canvas->cd();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(56);
  histogram_->Draw("colz");
  canvas->SaveAs(outputFile.c_str());
  filenames->push_back(outputFile);
}

} /* namespace comptonsoft */
