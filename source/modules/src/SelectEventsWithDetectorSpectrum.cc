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

#include "SelectEventsWithDetectorSpectrum.hh"

#include <random>
#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

SelectEventsWithDetectorSpectrum::SelectEventsWithDetectorSpectrum()
{
}

SelectEventsWithDetectorSpectrum::~SelectEventsWithDetectorSpectrum() = default;

ANLStatus SelectEventsWithDetectorSpectrum::mod_define()
{
  define_parameter("exposure", &mod_class::exposure_);
  define_parameter("energy_array", &mod_class::energyArray_, unit::keV, "keV");
  define_parameter("count_rate_array", &mod_class::countRateArray_);
  define_parameter("good_grade", &mod_class::goodGrade_);

  return AS_OK;
}

ANLStatus SelectEventsWithDetectorSpectrum::mod_initialize()
{
  get_module_NC("CSHitCollection", &hitCollection_);
  get_module_NC("ReadEventTree", &readEventTree_);

  const int n = countRateArray_.size();
  const int m = energyArray_.size();
  if (m!=n+1) {
    return AS_ERROR;
  }
  photonStack_.resize(n, 0);
  numRemainedBin_ = n;
  for (int i=0; i<n; i++) {
    const int v = static_cast<int>(countRateArray_[i] * exposure_ + 0.5);
    countArray_.push_back(v);
  }
  for (int i=0; i<n; i++) {
    if (countArray_[i]==0) {
      numRemainedBin_-=1;
    }
  }

  return AS_OK;
}

ANLStatus SelectEventsWithDetectorSpectrum::mod_analyze()
{  
  if (numRemainedBin_==0) {
    return AS_QUIT;
  }

  const int timeGroup = 0;
  double energy = 0.0;
  std::vector<DetectorHit_sptr>& hits = hitCollection_->getHits(timeGroup);
  DetectorHit_sptr hit0 = hits[0];
  energy = hit0->Energy();
  const int grade = hit0->Grade();

  if (grade>=0) {
    const std::vector<double>::const_iterator it = std::upper_bound(std::begin(energyArray_), std::end(energyArray_), energy);
    const int bin = it - energyArray_.begin() - 1;
    if (it==energyArray_.begin() || it==energyArray_.end()) {
      hitCollection_->initializeEvent();
    }
    else if (photonStack_[bin]>=countArray_[bin]) {
      hitCollection_->initializeEvent();
    }
    else {
      if (isGoodGrade(grade)) {
        photonStack_[bin] += 1;
      }
      if (photonStack_[bin]==countArray_[bin]) {
        numRemainedBin_ -= 1;
      }
    }
  }

  return AS_OK;
}

ANLStatus SelectEventsWithDetectorSpectrum::mod_end_run()
{
  std::cout << "Remained bin: " << numRemainedBin_ << std::endl;
  const int n = photonStack_.size();
  for (int i=0; i<n; i++) {
    if (photonStack_[i]<countArray_[i]) {
      std::cout << energyArray_[i]/unit::keV << " " << energyArray_[i+1]/unit::keV << " " << countArray_[i] << " " << photonStack_[i] << " " << countArray_[i]-photonStack_[i] << std::endl;
    }
  }

  return AS_OK;
}

bool SelectEventsWithDetectorSpectrum::isGoodGrade(int grade)
{
  const int n = goodGrade_.size();
  for (int i=0; i<n; i++) {
    if (grade==goodGrade_[i]) {
      return true;
    }
  }
  return false;
}


} /* namespace comptonsoft */
