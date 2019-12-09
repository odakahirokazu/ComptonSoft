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

#include "AssignSXIGrade.hh"

#include <random>
#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"
#include "SXIXrayEvent.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

AssignSXIGrade::AssignSXIGrade()
{
}

AssignSXIGrade::~AssignSXIGrade() = default;

ANLStatus AssignSXIGrade::mod_define()
{
  define_parameter("event_threshold", &mod_class::eventThreshold_, unit::keV, "keV");
  define_parameter("split_threshold", &mod_class::splitThreshold_, unit::keV, "keV");

  return AS_OK;
}

ANLStatus AssignSXIGrade::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &hitCollection_);
  image_.resize(boost::extents[outerSize_][outerSize_]);
  if (outerSplitThreshold_==0.0) {
    outerSplitThreshold_ = splitThreshold_;
  }

  return AS_OK;
}

ANLStatus AssignSXIGrade::mod_analyze()
{
  resetImage(image_);
  SXIXrayEvent_sptr event(new SXIXrayEvent(outerSize_));
  const int halfSize = outerSize_/2;
  const int timeGroup = 0;

  std::vector<DetectorHit_sptr>& hits = hitCollection_->getHits(timeGroup);
  int grade = -1;
  double sumPH = 0.0;
  double maxPH = 0.0;
  int cx = 0;
  int cy = 0;
  for (DetectorHit_sptr hit: hits) {
    const double ph = hit->EPI();
    if (ph>maxPH) {
      cx = hit->PixelX();
      cy = hit->PixelY();
      maxPH = ph;
    }
  }
  if (maxPH>eventThreshold_) {
    image_[halfSize][halfSize] = maxPH;
    for (DetectorHit_sptr hit:hits) {
      const double ph = hit->EPI();
      const int ix = hit->PixelX();
      const int iy = hit->PixelY();
      if (std::abs(ix-cx)<=2 && std::abs(iy-cy)<=2 && (ix!=cx || iy!=cy)) {
        image_[ix-cx+halfSize][iy-cy+halfSize] = ph;
       }
    }
    event->setSplitThreshold(splitThreshold_);
    event->setOuterSplitThreshold(outerSplitThreshold_);
    event->copyFrom(image_, halfSize, halfSize);
    event->reduce();
    grade = event->Grade();
    sumPH = event->SumPH();
  }
  for (DetectorHit_sptr hit:hits) {
    hit->setGrade(grade);
    hit->setEnergy(sumPH);
  }
  
  return AS_OK;
}

void AssignSXIGrade::resetImage(image_t& image)
{
  const int nx = image.shape()[0];
  const int ny = image.shape()[1];
  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      image[ix][iy] = 0.0;
    }
  }
}

} /* namespace comptonsoft */
