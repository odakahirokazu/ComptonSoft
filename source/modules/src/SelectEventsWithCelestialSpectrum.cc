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

#include "SelectEventsWithCelestialSpectrum.hh"

#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"
#include "Randomize.hh"

namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

namespace
{

/**
 * calculate overlap of two sections.
 */
double overlap(double v1, double v2, double w1, double w2);

} /* anonymous namespace */

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

SelectEventsWithCelestialSpectrum::SelectEventsWithCelestialSpectrum()
{
}

SelectEventsWithCelestialSpectrum::~SelectEventsWithCelestialSpectrum() = default;

ANLStatus SelectEventsWithCelestialSpectrum::mod_define()
{
  define_parameter("exposure", &mod_class::exposure_);
  define_parameter("energy_array", &mod_class::energyArray_, unit::keV, "keV");
  define_parameter("photons_array", &mod_class::photonsArray_, 1.0/(unit::s*unit::cm*unit::cm), "1.0/(s*cm*cm)");
  define_parameter("arf_filename", &mod_class::arfFilename_);
  define_parameter("assign_position", &mod_class::assignPosition_);

  return AS_OK;
}

ANLStatus SelectEventsWithCelestialSpectrum::mod_initialize()
{
  get_module_NC("CSHitCollection", &hitCollection_);
  get_module_NC("ReadEventTree", &readEventTree_);

  ANLStatus status = AS_OK;
  if (assignPosition_) {
    inputImage(arfFilename_, image_, &status);
  }
  if (status != AS_OK) {
    return status;
  }

  const int n = photonsArray_.size();
  const int m = energyArray_.size();
  if (m!=n+1) {
    std::cout << "ERROR, photons_array size is " << n << " and energy_array size is " << m << std::endl;
    return AS_ERROR;
  }

  inputEffectiveArea(arfFilename_, effectiveArea_, &status);
  if (status != AS_OK) {
    return status;
  }

  countArray_.resize(n);

  for (int i=0; i<n; i++){
    const double dE = energyArray_[i+1] - energyArray_[i];
    double v = 0.0;
    const int numPlot = effectiveArea_.size();
    for (int j=0; j<numPlot; j++){
      v += photonsArray_[i] * effectiveArea_[j].area * overlap(energyArray_[i], energyArray_[i+1], effectiveArea_[j].emin, effectiveArea_[j].emax)/dE;
    }
    countArray_[i] = static_cast<int>(v * exposure_ + 0.5);
  }

  photonStack_.resize(n, 0);
  numRemainedBin_ = n;

  for (int i=0; i<n; i++) {
    if (countArray_[i]==0) {
      numRemainedBin_ -= 1;
    }
  }

  buildPositionIntegral();

  return AS_OK;
}

ANLStatus SelectEventsWithCelestialSpectrum::mod_analyze()
{  
  if (numRemainedBin_==0) {
    return AS_QUIT;
  }
 
  double initialEnergy = readEventTree_->InitialEnergy();

  const std::vector<double>::const_iterator it = std::upper_bound(energyArray_.begin(), energyArray_.end(), initialEnergy);
  const int bin = it - energyArray_.begin() - 1;
  if (it==energyArray_.begin() || it==energyArray_.end()) {
    hitCollection_->initializeEvent();
  }
  else if (photonStack_[bin]>=countArray_[bin]) {
    hitCollection_->initializeEvent();
  }
  else {
    photonStack_[bin] += 1;
    if (photonStack_[bin]==countArray_[bin]) {
      numRemainedBin_ -= 1;
    }
    if (assignPosition_) {
      setPosition();
    }
  }

  return AS_OK;
}

ANLStatus SelectEventsWithCelestialSpectrum::mod_end_run()
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

void SelectEventsWithCelestialSpectrum::inputImage(std::string filename, image_t& image, ANLStatus* status)
{
  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  *status = AS_OK;
  int maxdim = 2;
  long naxes[2] = {0, 0};
  long startPixel[2] = {1, 1};
  int numPixels = 1;
  int nulval = 0;
  int anynul = 0;
  std::vector<double> array;

  cfitsio::fits_open_image(&fitsFile, filename.c_str(), READONLY, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }
  cfitsio::fits_get_img_size(fitsFile, maxdim, naxes, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }
  image.resize(boost::extents[naxes[0]][naxes[1]]);
  numPixels = naxes[0] * naxes[1];
  array.resize(numPixels, 0.0);

  cfitsio::fits_read_pix(fitsFile, TDOUBLE, startPixel, numPixels, &nulval, &array[0], &anynul, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }

  int i = 0;
  for (int ix=0; ix<naxes[0]; ix++) {
    for (int iy=0; iy<naxes[1]; iy++) {
      image[ix][iy] = array[i];
      i++;
    }
  }

  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }
}

void SelectEventsWithCelestialSpectrum::inputEffectiveArea(std::string filename, std::vector<Effarea>& effarea, ANLStatus* status)
{
  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  int anynul = 0;
  double doublenul = 0.0;
  int nfound = 0;
  long naxes[2] = {0, 0};
  static const int numColumns = 3;

  std::string colname[numColumns] = {"ENERG_LO", "ENERG_HI", "SPECRESP"};
  int colid[numColumns] = {0};

  cfitsio::fits_open_image(&fitsFile, filename.c_str(), READONLY, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }

  cfitsio::ffmahd(fitsFile, 2, IMAGE_HDU, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }

  for(int i=0; i<numColumns; i++) {
    cfitsio::fits_get_colnum(fitsFile, CASEINSEN, const_cast<char*>(colname[i].c_str()), &colid[i], &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      *status = AS_QUIT_ERROR;
      return;
    }
  }

  cfitsio::fits_read_keys_lng(fitsFile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }
  const int numPlot = static_cast<int>(naxes[1]);
  effarea.resize(numPlot);

  for(int i=0; i<numColumns; i++){
    std::vector<double> v(numPlot);
    cfitsio::fits_read_col(fitsFile, TDOUBLE, colid[i], (long)1, (long)1, naxes[1], &doublenul, &(v[0]), &anynul, &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      *status = AS_QUIT_ERROR;
      return;
    }
    for (int j=0; i<numPlot; i++) {
      if (i==0) effarea[j].emin = v[j];
      else if (i==1) effarea[j].emax = v[j];
      else if (i==2) effarea[j].area = v[j];
    }
  }

  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    *status = AS_QUIT_ERROR;
    return;
  }

  for (int i=0; i<numPlot; i++){
    effarea[i].emin *= unit::keV;
    effarea[i].emax *= unit::keV;
    effarea[i].area *= (unit::cm*unit::cm);
  }
}

void SelectEventsWithCelestialSpectrum::buildPositionIntegral()
{
  const int nx = image_.shape()[0];
  const int ny = image_.shape()[1];
  const int nbins = nx*ny;
  positionIntegral_.resize(nbins+1);
  positionIntegral_[0] = 0.0;

  for (int i=0; i<nbins; i++) {
    positionIntegral_[i+1] = positionIntegral_[i] + image_[i%nx][i/nx];
  }

  const double norm = positionIntegral_.back();
  for (auto& v: positionIntegral_) {
    v /= norm;
  }
}

void SelectEventsWithCelestialSpectrum::setPosition()
{
  const int timeGroup = 0;
  std::pair<int, int> pixel = samplePixel();
  const int nx = pixel.first;
  const int ny = pixel.second;

  std::vector<DetectorHit_sptr>& hits = hitCollection_->getHits(timeGroup);
  double maxPH = 0.0;
  int cx = 0;
  int cy = 0;
  for (auto hit: hits) {
    if (hit->Energy()>maxPH) {
      maxPH = hit->Energy();
      cx = hit->PixelX();
      cy = hit->PixelY();
    }
  }

  for (auto hit: hits) {
    const int x = nx + (hit->PixelX() - cx);
    const int y = ny + (hit->PixelY() - cy);
    hit->setPixel(x, y);
  }
}

std::pair<int, int> SelectEventsWithCelestialSpectrum::samplePixel()
{
  std::pair<int, int> res;
  const int nx = image_.shape()[0];

  const double r = G4UniformRand();
  const std::vector<double>::const_iterator it = std::upper_bound(positionIntegral_.begin(), positionIntegral_.end(), r);
  const int r0 = it - positionIntegral_.begin() - 1;
  res.first = r0%nx;
  res.second = r0/nx;

  return res;
}

} /* namespace comptonsoft */

namespace
{
double overlap(double v1, double v2, double w1, double w2)
{
  if (v1>v2 || w1>w2){
    return 0.0;
  }
  else if (v2<w1 || w2<v1){
    return 0.0;
  }
  else {
    std::vector<double> v;
    v.push_back(v1);
    v.push_back(v2);
    v.push_back(w1);
    v.push_back(w2);
    std::sort(v.begin(), v.end());
    return v[2] - v[1];
  }
}

} /* anonymous namespace */
