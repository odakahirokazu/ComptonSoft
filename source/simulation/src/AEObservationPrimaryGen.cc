/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#include "AEObservationPrimaryGen.hh"
#include "AstroUnits.hh"
#include "Randomize.hh"

namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

using namespace anlnext;
using namespace anlgeant4;

namespace comptonsoft
{

AEObservationPrimaryGen::AEObservationPrimaryGen()
  : arfFileName_("source.arf")    
{
  add_alias("AEObservationPrimaryGen");
}

ANLStatus AEObservationPrimaryGen::mod_define()
{
  anlnext::ANLStatus status = BasicPrimaryGen::mod_define();
  if (status!=AS_OK) {
    return status;
  }  

  setParticleName("gamma");
  
  register_parameter(&offset_, "position_offset", unit::mm, "mm");
  register_parameter(&arfFileName_, "arf_filename");
  register_parameter(&pixelSize_, "pixel_size", unit::mm, "mm");
  register_parameter(&pixelX_, "num_pixel_x");
  register_parameter(&pixelY_, "num_pixel_y");
  register_parameter(&exposure_, "exposure", unit::s, "s");
  register_parameter(&useFlux_, "use_flux");
  if (useFlux_) {
    register_parameter(&flux_, "flux", unit::erg/(unit::cm*unit::cm*unit::s), "erg s^-1 cm^-2");
    register_parameter(&fluxEnergyMin_, "flux_energy_min", unit::keV, "keV");
    register_parameter(&fluxEnergyMax_, "flux_energy_max", unit::keV, "keV");
  }

  return AS_OK;
}

ANLStatus AEObservationPrimaryGen::mod_pre_initialize()
{
  ANLStatus status = BasicPrimaryGen::mod_pre_initialize();
  if (status!=AS_OK) {
    return status;
  }

  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  long StartPixel[2] = {1, 1};
  const int nx = pixelX_;
  const int ny = pixelY_;
  const long  NumPixels = nx * ny;
  int nulval = 0;
  int anynul = 0;
  double doublenul = 0.0;
  int nfound = 0;
  long naxes[2] = {0, 0};

  std::vector<double> array;
  array.resize(NumPixels, 0.0);
  PSFArray_.resize(NumPixels, 0.0);
  PSF_.resize(boost::extents[nx][ny]);

  std::string colname[numColumns_] = {"ENERG_LO", "ENERG_HI", "SPECRESP"};
  int colid[numColumns_] = {0};

  cfitsio::fits_open_image(&fitsFile, arfFileName_.c_str(), READONLY, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }
 
  cfitsio::fits_read_pix(fitsFile, TDOUBLE, StartPixel, NumPixels, &nulval, &array[0], &anynul, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::ffmahd(fitsFile, 2, IMAGE_HDU, &fitsStatus);
  if(fitsStatus){
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  for(int i=0; i<numColumns_; i++) {
    cfitsio::fits_get_colnum(fitsFile, CASEINSEN, const_cast<char*>(colname[i].c_str()),
                             &colid[i], &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      return AS_ERROR;
    }
  }
  
  cfitsio::fits_read_keys_lng(fitsFile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fitsStatus);
  numEffectiveAreaPlot_ = static_cast<int>(naxes[1]);
  
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_QUIT_ERROR;
  }
 
  for(int i=0; i<numColumns_; i++){
    effectiveArea_[i].resize(numEffectiveAreaPlot_);
    cfitsio::fits_read_col(fitsFile, TDOUBLE, colid[i], (long)1, (long)1,
                           naxes[1], &doublenul,
                           &(effectiveArea_[i][0]), &anynul, &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      return AS_ERROR;
    }
  }
  
  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  for (int iy=0; iy<ny; iy++){
    for (int ix=0; ix<nx; ix++){
      if (array[iy*nx+ix]>0.0) {
        PSF_[ix][iy] = array[iy*nx+ix];
      }
    }
  }

  for (int i=0; i<NumPixels; i++){
    if (array[i]>0.0) {
      PSFArray_[i] = array[i];
    }
  }

  for (int i=0; i<numEffectiveAreaPlot_; i++){
    effectiveArea_[0][i] *= unit::keV;
    effectiveArea_[1][i] *= unit::keV;
    effectiveArea_[2][i] *= (unit::cm*unit::cm);
  }

  const std::vector<double>& spectrumEnergy = SpectrumEnergy();
  const std::vector<double>& spectrumPhotons = SpectrumPhotons();

  if (!useFlux_) {
    fluxEnergyMin_ = spectrumEnergy[0];
    fluxEnergyMax_ = spectrumEnergy[spectrumEnergy.size()-1];
    for (size_t i=0; i<spectrumPhotons.size(); i++) {
      const double y = spectrumPhotons[i] / (unit::cm * unit::cm * unit::s);
      const double energy = 0.5 * (spectrumEnergy[i+1] + spectrumEnergy[i]);
      flux_ += y * energy;
    }
  }

  std::vector<double> newSpectrumPhotons;
  newSpectrumPhotons.resize(spectrumPhotons.size(), 0.0);

#if 0
  const double arfEnergyMin = effectiveArea_[0][0];
  const double arfEnergyMax = effectiveArea_[1].back();

  if (arfEnergyMin>spectrumEnergy[0] || arfEnergyMax<spectrumEnergy.back()) {
  std::cout << "The energy range of arf does not match to the spectrum." << std::endl;
  return AS_ERROR;
  }
#endif

  for (size_t i=0; i<spectrumPhotons.size(); i++){
    const double dE = spectrumEnergy[i+1] - spectrumEnergy[i];
    double v = 0.0;
    for (size_t j=0; j<effectiveArea_[0].size(); j++){
      v += spectrumPhotons[i] * effectiveArea_[2][j] * overlap(spectrumEnergy[i], spectrumEnergy[i+1], effectiveArea_[0][j], effectiveArea_[1][j])/dE;
    }
    newSpectrumPhotons[i] = v;
  }

  setSpectrumPhotons(newSpectrumPhotons);
  
  return AS_OK;
}

ANLStatus AEObservationPrimaryGen::mod_initialize()
{
  ANLStatus status = BasicPrimaryGen::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }
  buildPositionIntegral();
  return AS_OK;
}

ANLStatus AEObservationPrimaryGen::mod_analyze()
{
  if (sumFlux_ > flux_) {
    return AS_QUIT;
  }

/*
  if (sumFlux_ > flux_*percent_/100.0) {
    std::cout << percent_ << "\% completed." << std::endl;
    percent_ += 1;
  }
*/  
  
  return BasicPrimaryGen::mod_analyze();
}

void AEObservationPrimaryGen::makePrimarySetting()
{  
  const double energy = sampleEnergy();
  const G4ThreeVector position = samplePosition() + offset_;
  const G4ThreeVector direction(0.0, 0.0, -1.0);
  
  double area = 1.0;
  if (energy>fluxEnergyMin_ && energy<fluxEnergyMax_) {
    for (int i=0; i<numEffectiveAreaPlot_; i++){
      if (energy>effectiveArea_[0][i] && energy<effectiveArea_[1][i]) {
        area = effectiveArea_[2][i];
        break;
      }
    }
    sumFlux_ += energy/(exposure_*area);
  }

  setPrimary(position, energy, direction.unit());
}

void AEObservationPrimaryGen::buildPositionIntegral()
{
  const int nx = PSF_.shape()[0];
  const int ny = PSF_.shape()[1];
  const int nbins = nx*ny;
  positionIntegral_.resize(nbins+1);
  positionIntegral_[0] = 0.0;

  for (int i=0; i<nbins; i++) {
    positionIntegral_[i+1] = positionIntegral_[i] + PSFArray_[i];
  }

  const double norm = positionIntegral_.back();
  for (auto& v: positionIntegral_) {
    v /= norm;
  }
}

G4ThreeVector AEObservationPrimaryGen::samplePosition()
{
  G4ThreeVector position (0.0, 0.0, 0.0);
  const int nx = pixelX_;
  const int ny = pixelY_;
  const double halfSizeX = nx/2.0;
  const double halfSizeY = ny/2.0;
  const double pixelSize = pixelSize_;

  const std::vector<double>& integrals = positionIntegral_;
  const double r = G4UniformRand();
  const std::vector<double>::const_iterator it = std::upper_bound(std::begin(integrals), std::end(integrals), r);
  const int r0 = it - integrals.begin();
  const int ix = r0%nx;
  const int iy = r0/nx;
  const double dix = G4UniformRand();
  const double diy = G4UniformRand();

  const double x = (ix+dix-halfSizeX) * pixelSize * (-1.0);
  const double y = (iy+diy-halfSizeY) * pixelSize;
  const double z = 0.0;

  position = G4ThreeVector(x, y, z);
  return position;
}

double AEObservationPrimaryGen::overlap(double v1, double v2, double w1, double w2)
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

} /* namespace comptonsoft */
