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

#include "SemiconductorModel.hh"

#include <iostream>
#include "AstroUnits.hh"
#include "NumericalField.hh"
#include "WeightingPotentialPixel.hh"

namespace unit = anlgeant4::unit;
namespace constant = anlgeant4::constant;

namespace comptonsoft {

SemiconductorModel::SemiconductorModel()
  : pixelWidthX_(36.0*unit::um),
    pixelWidthY_(36.0*unit::um),
    thickness_(500.0*unit::um),
    mobility_( 5.0e+2 * (unit::cm2/unit::s/unit::volt) ),
    lifetime_( 10.0 * unit::us),
    temperature_( 273.15*unit::kelvin ),
    spreadFactor_(1.0),
    saturationMode_(false),
    saturationEField_( 2.0e+4 * (unit::volt/unit::cm))
{
  // D = kTmu/q
  diffusionCoefficient_ = constant::k_Boltzmann*temperature_*mobility_/constant::eplus;
}

SemiconductorModel::~SemiconductorModel()
{
  if (file_.get()) {
    file_->Close();
  }
}

void SemiconductorModel::setPixelDimensions(double x, double y, double z)
{
  pixelWidthX_ = x;
  pixelWidthY_ = y;
  thickness_ = z;
}

void SemiconductorModel::setProperties(const bool carrier_positive,
                                       const double mobility,
                                       const double lifetime,
                                       const double temperature,
                                       const double spreadFactor)
{
  carrierPositive_ = carrier_positive;
  mobility_ = mobility;
  lifetime_ = lifetime;
  temperature_ = temperature;
  spreadFactor_ = spreadFactor;

  // D = kTmu/q
  diffusionCoefficient_ = constant::k_Boltzmann*temperature_*mobility_/constant::eplus;
}

void SemiconductorModel::setVelocitySaturation(double efield)
{
  saturationMode_ = true;
  saturationEField_ = efield;
}

void SemiconductorModel::unsetVelocitySaturation()
{
  saturationMode_ = false;
  saturationEField_ = 0.0;
}

void SemiconductorModel::load(const std::string& filename)
{
  file_ = std::make_unique<TFile>(filename.c_str());
  fx_ = static_cast<TH3D*>(file_->Get("efieldx"));
  fy_ = static_cast<TH3D*>(file_->Get("efieldy"));
  fz_ = static_cast<TH3D*>(file_->Get("efieldz"));
}

void SemiconductorModel::
useAccurateWeightingPotential(const std::vector<double>& grids_x,
                              const std::vector<double>& grids_y,
                              const std::vector<double>& grids_z)
{
  auto WPModel = std::make_unique<WeightingPotentialPixel>();
  WPModel->setGeometry(pixelWidthX_, pixelWidthY_, thickness_);
  WPModel->initializeTable();

  WPField_ = std::make_unique<NumericalField>();
  WPField_->defineGrids(grids_x, grids_y, grids_z);
  WPField_->setZeros();
  const std::size_t nx = WPField_->num_x();
  const std::size_t ny = WPField_->num_y();
  const std::size_t nz = WPField_->num_z();

  std::cout << "Calculating weighting potentials... for "
            << nx << " x " << ny << " x " << nz << " points"
            << std::endl;
  for (std::size_t ix=0; ix<nx; ix++) {
    std::cout << "*"; std::cout.flush();
    for (std::size_t iy=0; iy<ny; iy++) {
      for (std::size_t iz=0; iz<nz; iz++) {
        const vector3_t position = WPField_->get_position(ix, iy, iz);
        double wp = 0.0;
        if (isUpsideReadout()) {
          if (iz==nz-1) { // readout electrode
            if (std::abs(position.x()) <= 0.5*pixelWidthX_ && std::abs(position.y()) <= 0.5*pixelWidthY_) {
              wp = 1.0;
            }
            else {
              wp = 0.0;
            }
          }
          else {
            wp = WPModel->calculateWeightingPotential(position.x(), position.y(), position.z());
          }
        }
        else {
          if (iz==0) { // readout electrode
            if (std::abs(position.x()) <= 0.5*pixelWidthX_ && std::abs(position.y()) <= 0.5*pixelWidthY_) {
              wp = 1.0;
            }
            else {
              wp = 0.0;
            }
          }
          else {
            wp = WPModel->calculateWeightingPotential(position.x(), position.y(), -position.z());
          }
        }
#if 0
        std::cout << "    wp = " << wp << " for z = " << position.z()/unit::um <<std::endl;
#endif
        WPField_->set_field_value(ix, iy, iz, wp);
      }
    }
  }
  std::cout << std::endl;
}

void SemiconductorModel::setRandomSeed(uint32_t seed)
{
  randomGenerator_.seed(seed);
}

vector3_t SemiconductorModel::EField(const vector3_t& position) const
{
  const int binx = fx_->GetXaxis()->FindBin(position.x()/unit::cm);
  const int biny = fx_->GetYaxis()->FindBin(position.y()/unit::cm);
  const int binz = fx_->GetZaxis()->FindBin(position.z()/unit::cm);
  const double ex = fx_->GetBinContent(binx, biny, binz) * unit::volt/unit::cm;
  const double ey = fy_->GetBinContent(binx, biny, binz) * unit::volt/unit::cm;
  const double ez = fz_->GetBinContent(binx, biny, binz) * unit::volt/unit::cm;
  return vector3_t(ex, ey, ez);
}

vector3_t SemiconductorModel::ChargeVelocity(const vector3_t& position) const
{
  vector3_t velocity;
  if (SaturationMode()) {
    const vector3_t Evec = EField(position);
    const double E = Evec.mag();
    const double Es = SaturationEField();
    const double muEffective = Mobility()*Es/(E+Es);
    velocity = muEffective*Evec;
  }
  else {
    const vector3_t Evec = EField(position);
    velocity = Mobility()*Evec;
  }

  if (isCarrierNegative()) {
    velocity *= -1.0;
  }

  return velocity;
}

double SemiconductorModel::WeightingPotential(const vector3_t& position) const
{
  const double x = position.x();
  const double y = position.y();
  const double z = position.z();

  if (WPField_.get()) {
    return WPField_->field(x, y, z);
  }

  if (std::abs(x) >= 0.5*pixelWidthX_ || std::abs(y) >= 0.5*pixelWidthY_) {
    return 0.0;
  }
  return WeightingPotentialPlaneParallel(z);
}

double SemiconductorModel::WeightingPotentialPlaneParallel(double z) const
{
  const int nz = fx_->GetNbinsZ();
  const double z0 = fx_->GetZaxis()->GetBinLowEdge(1) * unit::cm;
  const double z1 = fx_->GetZaxis()->GetBinUpEdge(nz) * unit::cm;

  double wp = 0.0;
  if (isUpsideReadout()) {
    wp = (z-z0)/(z1-z0);
  }
  else {
    wp = (z1-z)/(z1-z0);
  }

  return wp;
}

vector3_t SemiconductorModel::Diffusion(const double t)
{
  const double D = DiffusionCoefficient();
  const double spreadFactor = SpreadFactor();
  const double sigma = std::sqrt(2.0*D*t) * spreadFactor;
  std::normal_distribution<double> sampleGaussian(0.0, sigma);
  const double dx = sampleGaussian(randomGenerator_);
  const double dy = sampleGaussian(randomGenerator_);
  const double dz = sampleGaussian(randomGenerator_);
  return vector3_t(dx, dy, dz);
}

} /* namespace comptonsoft */
