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

#ifndef COMPTONSOFT_EFieldModel_H
#define COMPTONSOFT_EFieldModel_H 1

#include <boost/shared_array.hpp>

namespace comptonsoft {

/**
 * A class of an electric field inside the semiconductor detector.
 * @author Hirokazu Odaka
 * @date 2009-07-07
 * @date 2009-12-03 : fixed CCE calculation
 */
class EFieldModel
{
public:
  enum class FieldShape { Constant=1, Linear=2, Bending=3 };

public:
  EFieldModel();
  virtual ~EFieldModel() = default;
  EFieldModel(const EFieldModel&) = default;
  EFieldModel(EFieldModel&&) = default;
  EFieldModel& operator=(const EFieldModel&) = default;
  EFieldModel& operator=(EFieldModel&&) = default;

  void setUpSideAnode(bool val=true) { upsideAnode_ = val; }
  void setUpSideCathode(bool val=true) { upsideAnode_ = !val; }
  bool isUpSideAnode() const { return upsideAnode_; }
  bool isUpSideCathode() const { return !upsideAnode_; }

  void setUpSideReadElectrode(bool val=true) { upsideReadElectrode_ = val; }
  bool isUpSideReadElectrode() const { return upsideReadElectrode_; }

  void setThickness(double val) { thickness_ = val; }
  double Thickness() const { return thickness_; }

  void setMuTau(double mutau_e, double mutau_h)
  {
    mutauElectron_ = mutau_e;
    mutauHole_ = mutau_h;
  }
  void setMuTauElectron(double val) { mutauElectron_ = val; }
  void setMuTauHole(double val) { mutauHole_ = val; }
  double MuTauElectron() const { return mutauElectron_; }
  double MuTauHole()const { return mutauHole_; }

  bool setEField(double bias, FieldShape mode);
  bool setEField(double bias, FieldShape mode, double p0);
  bool setEField(double bias, FieldShape mode, double p0, double p1);
  bool setEField(double bias, FieldShape mode, double p0, double p1, double p2);
  bool setEField(double bias, FieldShape mode, double p0, double p1, double p2, double p3);
  double BiasVoltage() const { return biasVoltage_; }
  FieldShape EFieldMode() const { return EFieldMode_; }
  double EFieldParam(int i) const { return EFieldParam_[i]; }

  void setWeightingPotential(boost::shared_array<double> wp, int num_points)
  {
    numDivisions_ = num_points-1;
    weightingPotential_ = wp;
  }
  int NumPoints() const { return numDivisions_+1; }
  int NumDivisions() const { return numDivisions_; }

  double WeightingPotential(double z) const;
  double WeightingPotentialByIndex(int i) const;

  double PositionZ(double index) const;
  double PositionIndex(double z) const;

  double CCE(double z) const;
  double CCEOfElectron(double z) const { return CCE_impl(true, z); }
  double CCEOfHole(double z) const { return CCE_impl(false, z); }
  double DriftTimeMobility(double z, double zInitial) const;

  double CCEByHecht(double z) const;

private:
  double CCE_impl(bool electron, double z) const;

private:
  // geometry
  bool upsideAnode_;
  double thickness_;
  bool upsideReadElectrode_;

  // charge transport
  double mutauElectron_;
  double mutauHole_;

  // Actual electric field
  double biasVoltage_;
  FieldShape EFieldMode_;
  double EFieldParam_[4];
  double EFieldMean_;
  double alpha_; // mode 2 (linear)
  double alpha1_, alpha2_, bendingPoint_, EFieldMax_; // mode 3 (bending)
  
  // Weighting potential
  int numDivisions_;
  boost::shared_array<double> weightingPotential_;
};

inline double EFieldModel::WeightingPotentialByIndex(int i) const
{
  if (i<0) { return weightingPotential_[0]; }
  else if (numDivisions_<i) { return weightingPotential_[numDivisions_]; }
  return weightingPotential_[i];
}

inline double EFieldModel::PositionZ(double index) const
{
  return thickness_*(-0.5 + index/numDivisions_);
}

inline double EFieldModel::PositionIndex(double z) const
{
  return (z/thickness_ + 0.5)*numDivisions_;
}

inline double EFieldModel::CCE(double z) const
{
  double cce = CCEOfElectron(z) + CCEOfHole(z);
  return cce;
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EFieldModel_H */
