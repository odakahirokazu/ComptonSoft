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

#include "EFieldModel.hh"

#include <iostream>
#include <cmath>
#include "AstroUnits.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

EFieldModel::EFieldModel()
  : upsideAnode_(false),
    thickness_(0.5*unit::mm),
    upsideReadElectrode_(true),
    mutauElectron_(5.0e-3*unit::cm2/unit::volt),
    mutauHole_(1.5e-4*unit::cm2/unit::volt),
    biasVoltage_(600*unit::volt),
    EFieldMode_(FieldShape::Constant),
    numDivisions_(127)
{
  EFieldParam_[0] = 0.0;
  EFieldParam_[1] = 0.0;
  EFieldParam_[2] = 0.0;
  EFieldParam_[3] = 0.0;

  EFieldMean_ = biasVoltage_/thickness_;
  alpha_ = 0.0;
  alpha1_ = 0.0;
  alpha2_ = 0.0;
  bendingPoint_ = 0.0;
  EFieldMax_ = 0.0;
}

double EFieldModel::WeightingPotential(double z) const
{
  double index = PositionIndex(z);
  if (index<=0.0) {
    return weightingPotential_[0];
  }
  else if (index>=numDivisions_) {
    return weightingPotential_[numDivisions_];
  }
      
  int i = index;
  double r = index-i;
  return (1-r)*weightingPotential_[i] + r*weightingPotential_[i+1];
}
  
double EFieldModel::CCEByHecht(double z) const
{
  using std::exp;
  
  double zAnode, zCathode;
  if (isUpSideAnode()) {
    zAnode = +0.5*Thickness();
    zCathode = -0.5*Thickness();
  }
  else {
    zCathode = +0.5*Thickness();
    zAnode = -0.5*Thickness();
  }

  const double cceElectron = MuTauElectron()*EFieldMean_/Thickness()*(1.-exp(-DriftTimeMobility(zAnode,z)/MuTauElectron()));
  const double cceHole = MuTauHole()*EFieldMean_/Thickness()*(1.-exp(-DriftTimeMobility(zCathode,z)/MuTauHole()));
  double cce = cceElectron + cceHole;
  return cce;
}

double EFieldModel::CCE_impl(bool electron, double z) const
{
  using std::exp;

  bool runUp = (electron==isUpSideAnode());

  double mutau;
  if (electron) {
    mutau = MuTauElectron();
  }
  else {
    mutau = MuTauHole();
  }
  
  const int nDiv = numDivisions_;
  const double zi = z;
  const double wpi = WeightingPotential(zi);
  const double ziIndex = PositionIndex(zi);

  double cce = 0.;
  if (ziIndex<=0.0 || ziIndex>=nDiv) { return cce; }
  
  if (runUp) {
    const int ki = ziIndex + 1;
    if (ki>nDiv) { return cce; }

    double deltaWP, z1, qEff;
    deltaWP = WeightingPotentialByIndex(ki) - wpi;
    z1 = 0.5*(zi+PositionZ(ki));
    qEff = exp(-DriftTimeMobility(z1,zi)/mutau);
    cce += qEff * deltaWP;

    for (int k=ki; k<nDiv; k++) {
      deltaWP = WeightingPotentialByIndex(k+1) - WeightingPotentialByIndex(k);
      z1 = PositionZ(k+0.5);
      qEff = exp(-DriftTimeMobility(z1,zi)/mutau);
      cce += qEff * deltaWP;
    }

    if (!isUpSideReadElectrode()) {
      cce *= -1.0;
    }
  }
  else {
    const int ki = ziIndex;
    if (ki<0) { return cce; }

    double deltaWP, z1, qEff;
    deltaWP = WeightingPotentialByIndex(ki) - wpi;
    z1 = 0.5*(zi+PositionZ(ki));
    qEff = exp(-DriftTimeMobility(z1,zi)/mutau);
    cce += qEff * deltaWP;
  
    for (int k=ki; k>0; k--) {
      deltaWP = WeightingPotentialByIndex(k-1) - WeightingPotentialByIndex(k);
      z1 = PositionZ(k-0.5);
      qEff = exp(-DriftTimeMobility(z1,zi)/mutau);
      cce += qEff * deltaWP;
    }

    if (isUpSideReadElectrode()) {
      cce *= -1.0;
    }
  }

  return cce;
}

bool EFieldModel::setEField(double bias, FieldShape mode)
{
  if (mode==FieldShape::Constant) {
    biasVoltage_ = bias;
    EFieldMode_ = mode;
    EFieldParam_[0] = 0.0;
    EFieldParam_[1] = 0.0;
    EFieldParam_[2] = 0.0;
    EFieldParam_[3] = 0.0;
    
    EFieldMean_ = biasVoltage_/thickness_;
    alpha_ = 0.;
    alpha1_ = 0.;
    alpha2_ = 0.;
    bendingPoint_ = 0.;
    EFieldMax_ = 0.;
  }
  else {
    std::cout << "Error: E field mode is invalid. " << std::endl;
    return false;
  }
  return true;
}

bool EFieldModel::setEField(double bias, FieldShape mode, double p0)
{
  using std::abs;

  if (mode==FieldShape::Linear) {
    biasVoltage_ = bias;
    EFieldMode_ = mode;
    EFieldParam_[0] = p0;
    EFieldParam_[1] = 0.0;
    EFieldParam_[2] = 0.0;
    EFieldParam_[3] = 0.0;
    
    if (abs(EFieldParam(0)-1.0)<0.0001) {
      std::cout << "Error: EFieldRatio ~ 1." << std::endl;
      return false;
    }

    if (isUpSideAnode()) {
      alpha_ = 0.5*Thickness()*(EFieldParam(0)+1.0)/(EFieldParam(0)-1.0);
    }
    else {
      alpha_ = 0.5*Thickness()*(1./EFieldParam(0)+1.0)/(1./EFieldParam(0)-1.0);
    }

    EFieldMean_ = biasVoltage_/thickness_;
    alpha1_ = 0.;
    alpha2_ = 0.;
    bendingPoint_ = 0.;
    EFieldMax_ = 0.;
  }
  else {
    std::cout << "Error: E field mode is invalid. " << std::endl;
    return false;
  }
  return true;
}

bool EFieldModel::setEField(double /* bias */, FieldShape /* mode */, double /* p0 */, double /* p1 */)
{
  std::cout << "Error: E field mode is invalid. " << std::endl;
  return false;
}

bool EFieldModel::setEField(double bias, FieldShape mode, double p0, double p1, double p2)
{
  using std::abs;
  
  if (mode==FieldShape::Bending) {
    biasVoltage_ = bias;
    EFieldMode_ = mode;
    EFieldParam_[0] = p0;
    EFieldParam_[1] = p1;
    EFieldParam_[2] = p2;
    EFieldParam_[3] = 0.0;

    if (abs(EFieldParam(0)-1.0)<0.0001 || abs(EFieldParam(1)-1.0)<0.0001) {
      std::cout << "Error: EFieldRatio ~ 1." << std::endl;
      return false;
    }

    if (isUpSideAnode()) {
      bendingPoint_ = (-0.5+EFieldParam(2))*Thickness();
      EFieldMax_ = 2.0*BiasVoltage()/((EFieldParam(0)-EFieldParam(1))*bendingPoint_+0.5*(2.0+EFieldParam(0)+EFieldParam(1))*Thickness());
      alpha1_ = (bendingPoint_+0.5*Thickness())/(1.-EFieldParam(0));
      alpha2_ = (bendingPoint_-0.5*Thickness())/(1.-EFieldParam(1));
    }
    else {
      bendingPoint_ = (0.5-EFieldParam(2))*Thickness();
      EFieldMax_ = 2.0*BiasVoltage()/((EFieldParam(1)-EFieldParam(0))*bendingPoint_+0.5*(2.0+EFieldParam(0)+EFieldParam(1))*Thickness());
      alpha1_ = (bendingPoint_+0.5*Thickness())/(1.-EFieldParam(1));
      alpha2_ = (bendingPoint_-0.5*Thickness())/(1.-EFieldParam(0));
    }

    EFieldMean_ = BiasVoltage()/Thickness(); 
    alpha_ = 0.;
  }
  else {
    std::cout << "Error: E field mode is invalid. " << std::endl;
    return false;
  }
  return true;
}

bool EFieldModel::setEField(double bias, FieldShape mode, double p0, double p1, double p2, double /* p3 */)
{
  if (mode==FieldShape::Constant) {
    return setEField(bias, mode);
  }
  else if (mode==FieldShape::Linear) {
    return setEField(bias, mode, p0);
  }
  else if (mode==FieldShape::Bending) {
    return setEField(bias, mode, p0, p1, p2);
  }

  std::cout << "Error: E field mode is invalid. " << std::endl;
  return false;
}

double EFieldModel::DriftTimeMobility(double z, double zInitial) const
{
  using std::abs;
  using std::log;
  
  double muT = 0.0;
  if (EFieldMode()==FieldShape::Constant) {
    muT = abs((z-zInitial)/EFieldMean_);
  }
  else if (EFieldMode()==FieldShape::Linear) {
    muT = abs(alpha_*log((z+alpha_)/(zInitial+alpha_))/EFieldMean_);
  }
  else if (EFieldMode()==FieldShape::Bending) {
    if (zInitial <= z) {
      if (bendingPoint_ <= zInitial) {
        muT = abs(alpha2_*log((z-bendingPoint_+alpha2_)/(zInitial-bendingPoint_+alpha2_)))/EFieldMax_;
      }
      else {
        if (z <= bendingPoint_) {
          muT = abs(alpha1_*log((z-bendingPoint_+alpha1_)/(zInitial-bendingPoint_+alpha1_)))/EFieldMax_;
        }
        else {
          muT = (abs(alpha1_*log(alpha1_/(zInitial-bendingPoint_+alpha1_))) + abs(alpha2_*log((z-bendingPoint_+alpha2_)/alpha2_)))/EFieldMax_;
        }
      }
    }
    else {
      if (bendingPoint_ >= zInitial) {
        muT = abs(alpha1_*log((z-bendingPoint_+alpha1_)/(zInitial-bendingPoint_+alpha1_)))/EFieldMax_;
      }
      else {
        if (z >= bendingPoint_) {
          muT = abs(alpha2_*log((z-bendingPoint_+alpha2_)/(zInitial-bendingPoint_+alpha2_)))/EFieldMax_;
        }
        else {
          muT = (abs(alpha2_*log(alpha2_/(zInitial-bendingPoint_+alpha2_))) + abs(alpha1_*log((z-bendingPoint_+alpha1_)/alpha1_)))/EFieldMax_;
        }
      }
    }
  }

  return muT;
}

} /* namespace comptonsoft */
