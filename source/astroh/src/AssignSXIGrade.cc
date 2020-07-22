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
  define_parameter("good_grade", &mod_class::goodGrade_);

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
  if (hits.size()==0) {
    return AS_OK;
  }
  auto comp = [](DetectorHit_sptr a, DetectorHit_sptr b) {
                return a->EPI() > b->EPI();
              };
  std::sort(hits.begin(), hits.end(), comp);
  
  int grade = -1;
  double sumPH = 0.0;
  double maxPH = hits[0]->EPI();
  const int cx = hits[0]->PixelX();
  const int cy = hits[0]->PixelY();

  if (maxPH>eventThreshold_) {
    image_[halfSize][halfSize] = maxPH;
    for (DetectorHit_sptr hit: hits) {
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

  hits[0]->setGrade(grade);
  if (isGoodGrade(grade)) {
    hits[0]->setEnergy(sumPH);
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

bool AssignSXIGrade::isGoodGrade(int grade) {
  for (int g: goodGrade_) {
    if (grade==g) return true;
  }
  return false;
}

} /* namespace comptonsoft */
