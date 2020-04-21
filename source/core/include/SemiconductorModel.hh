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

#ifndef COMPTONSOFT_SemiconductorModel_H
#define COMPTONSOFT_SemiconductorModel_H 1

#include <memory>
#include <random>
#include <vector>
#include <TH3.h>
#include <TFile.h>
#include "CSTypes.hh"

namespace comptonsoft {

class NumericalField;


/**
 * A semiconductor device model class for charge transport calculation
 *
 * @author Hiromasa Suzuki, Hirokazu Odaka
 * @date 2020-04-18
 */
class SemiconductorModel
{
public:
  SemiconductorModel();
  virtual ~SemiconductorModel();

  void setUpsideReadout(bool v) { upsideReadout_ = v; }
  void setPixelDimensions(double x, double y, double z);
  void setProperties(bool carrier_positive,
                     double mobility,
                     double lifetime,
                     double temperature,
                     double spreadFactor);
  void setVelocitySaturation(double efield);
  void unsetVelocitySaturation();
  void load(const std::string& filename);
  void useAccurateWeightingPotential(const std::vector<double>& grids_x,
                                     const std::vector<double>& grids_y,
                                     const std::vector<double>& grids_z);

  void setRandomSeed(uint32_t seed);

  bool isUpsideReadout() const { return upsideReadout_; }
  
  vector3_t EField(const vector3_t& position) const;
  vector3_t ChargeVelocity(const vector3_t& position) const;
  double WeightingPotential(const vector3_t& position) const;

  bool SaturationMode() const { return saturationMode_; }
  double SaturationEField() const { return saturationEField_; }

  bool isCarrierPositive() const { return carrierPositive_; }
  bool isCarrierNegative() const { return !isCarrierPositive(); }
  double Mobility() const { return mobility_; }
  double Lifetime() const { return lifetime_; }
  double DiffusionCoefficient() const { return diffusionCoefficient_; }
  double SpreadFactor() const { return spreadFactor_; }

  vector3_t Diffusion(double t);

protected:
  double WeightingPotentialPlaneParallel(double z) const;

private:
  bool upsideReadout_ = true;
  double pixelWidthX_ = 0.0;
  double pixelWidthY_ = 0.0;
  double thickness_ = 0.0;

  bool carrierPositive_ = true;
  double mobility_ = 0.0;
  double lifetime_ = 0.0;
  double temperature_ = 0.0;
  double spreadFactor_ = 0.0;
  double diffusionCoefficient_ = 0.0;

  bool saturationMode_ = false;
  double saturationEField_ = 0.0;
  
  std::unique_ptr<TFile> file_;
  TH3* fx_ = nullptr;
  TH3* fy_ = nullptr;
  TH3* fz_ = nullptr;

  std::unique_ptr<NumericalField> WPField_;
  
  std::mt19937 randomGenerator_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SemiconductorModel_H */
