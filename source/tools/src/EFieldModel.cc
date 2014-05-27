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

// Hiro Odaka

#include "EFieldModel.hh"

#include <iostream>
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "TMath.h"

namespace comptonsoft {


EFieldModel::EFieldModel()
  : m_UpsideAnode(false),
    m_Thickness(0.5*mm),
    m_UpsideReadElectrode(true),
    m_MuTauElectron(5.0e-3*cm2/volt),
    m_MuTauHole(1.5e-4*cm2/volt),
    m_BiasVoltage(600*volt),
    m_EFieldMode(1),
    m_NumDivision(127)
{
  m_EFieldParam[0] = 0.0;
  m_EFieldParam[1] = 0.0;
  m_EFieldParam[2] = 0.0;
  m_EFieldParam[3] = 0.0;

  m_EFieldMean = m_BiasVoltage/m_Thickness;
  m_Alpha = 0.;
  m_Alpha1 = 0.;
  m_Alpha2 = 0.;
  m_BendingPoint = 0.;
  m_EFieldMax = 0.;
}


double EFieldModel::WP(double z) const
{
  double index = PositionIndex(z);
  if (index<=0.0) {
    return m_WP[0];
  }
  else if (index>=m_NumDivision) {
    return m_WP[m_NumDivision];
  }
      
  int i = index;
  double r = index-i;
  return (1-r)*m_WP[i] + r*m_WP[i+1];
}
  

double EFieldModel::CCEByHecht(double z) const
{
  double zAnode, zCathode;
  if ( UpsideAnode() ) {
    zAnode = +0.5*Thickness();
    zCathode = -0.5*Thickness();
  }
  else {
    zCathode = +0.5*Thickness();
    zAnode = -0.5*Thickness();
  }

  double cceElectron = MuTauElectron()*m_EFieldMean/Thickness()*(1.-TMath::Exp(-DriftTimeMobility(zAnode,z)/MuTauElectron()));
  double cceHole = MuTauHole()*m_EFieldMean/Thickness()*(1.-TMath::Exp(-DriftTimeMobility(zCathode,z)/MuTauHole()));
  double cce = cceElectron + cceHole;
  return cce;
}


double EFieldModel::CCE_impl(bool electron, double z) const
{
  bool runUp = (electron==UpsideAnode());

  double mutau;
  if (electron) {
    mutau = MuTauElectron();
  }
  else {
    mutau = MuTauHole();
  }
  
  const int nDiv = m_NumDivision;
  const double zi = z;
  const double wpi = WP(zi);
  const double ziIndex = PositionIndex(zi);

  double cce = 0.;
  if (ziIndex<=0.0 || ziIndex>=nDiv) { return cce; }
  
  if (runUp) {
    const int ki = ziIndex + 1;
    if (ki>nDiv) { return cce; }

    double deltaWP, z1, qEff;
    deltaWP = WPByIndex(ki) - wpi;
    z1 = 0.5*(zi+PositionZ(ki));
    qEff = TMath::Exp(-DriftTimeMobility(z1,zi)/mutau);
    cce += qEff * deltaWP;

    for (int k=ki; k<nDiv; k++) {
      deltaWP = WPByIndex(k+1) - WPByIndex(k);
      z1 = PositionZ(k+0.5);
      qEff = TMath::Exp(-DriftTimeMobility(z1,zi)/mutau);
      cce += qEff * deltaWP;
    }

    if (!UpsideReadElectrode()) {
      cce *= -1.0;
    }
  }
  else {
    const int ki = ziIndex;
    if (ki<0) { return cce; }

    double deltaWP, z1, qEff;
    deltaWP = WPByIndex(ki) - wpi;
    z1 = 0.5*(zi+PositionZ(ki));
    qEff = TMath::Exp(-DriftTimeMobility(z1,zi)/mutau);
    cce += qEff * deltaWP;
  
    for (int k=ki; k>0; k--) {
      deltaWP = WPByIndex(k-1) - WPByIndex(k);
      z1 = PositionZ(k-0.5);
      qEff = TMath::Exp(-DriftTimeMobility(z1,zi)/mutau);
      cce += qEff * deltaWP;
    }

    if (UpsideReadElectrode()) {
      cce *= -1.0;
    }
  }

  return cce;
}


bool EFieldModel::setEField(double bias, int mode)
{
  if (mode==1) {
    m_BiasVoltage = bias;
    m_EFieldMode = mode;
    m_EFieldParam[0] = 0.0;
    m_EFieldParam[1] = 0.0;
    m_EFieldParam[2] = 0.0;
    m_EFieldParam[3] = 0.0;
    
    m_EFieldMean = m_BiasVoltage/m_Thickness; 
    m_Alpha = 0.;
    m_Alpha1 = 0.;
    m_Alpha2 = 0.;
    m_BendingPoint = 0.;
    m_EFieldMax = 0.;
  }
  else {
    std::cout << "Error: E field mode is invalid. " << std::endl;
    return false;
  }
  return true;
}


bool EFieldModel::setEField(double bias, int mode, double p0)
{
  if (mode==2) {
    m_BiasVoltage = bias;
    m_EFieldMode = mode;
    m_EFieldParam[0] = p0;
    m_EFieldParam[1] = 0.0;
    m_EFieldParam[2] = 0.0;
    m_EFieldParam[3] = 0.0;
    
    if (TMath::Abs(EFieldParam(0)-1.0)<0.0001) {
      std::cout << "Error: EFieldRatio ~ 1." << std::endl;
      return false;
    }

    if (UpsideAnode()) {
      m_Alpha = 0.5*Thickness()*(EFieldParam(0)+1.0)/(EFieldParam(0)-1.0);
    }
    else {
      m_Alpha = 0.5*Thickness()*(1./EFieldParam(0)+1.0)/(1./EFieldParam(0)-1.0);
    }

    m_EFieldMean = m_BiasVoltage/m_Thickness;
    m_Alpha1 = 0.;
    m_Alpha2 = 0.;
    m_BendingPoint = 0.;
    m_EFieldMax = 0.;
  }
  else {
    std::cout << "Error: E field mode is invalid. " << std::endl;
    return false;
  }
  return true;
}


bool EFieldModel::setEField(double /* bias */, int /* mode */, double /* p0 */, double /* p1 */)
{
  std::cout << "Error: E field mode is invalid. " << std::endl;
  return false;
}


bool EFieldModel::setEField(double bias, int mode, double p0, double p1, double p2)
{
  if (mode==3) {
    m_BiasVoltage = bias;
    m_EFieldMode = mode;
    m_EFieldParam[0] = p0;
    m_EFieldParam[1] = p1;
    m_EFieldParam[2] = p2;
    m_EFieldParam[3] = 0.0;

    if (TMath::Abs(EFieldParam(0)-1.0)<0.0001 || TMath::Abs(EFieldParam(1)-1.0)<0.0001) {
      std::cout << "Error: EFieldRatio ~ 1." << std::endl;
      return false;
    }

    if (UpsideAnode()) {
      m_BendingPoint = (-0.5+EFieldParam(2))*Thickness();
      m_EFieldMax = 2.0*BiasVoltage()/((EFieldParam(0)-EFieldParam(1))*m_BendingPoint+0.5*(2.0+EFieldParam(0)+EFieldParam(1))*Thickness());
      m_Alpha1 = (m_BendingPoint+0.5*Thickness())/(1.-EFieldParam(0));
      m_Alpha2 = (m_BendingPoint-0.5*Thickness())/(1.-EFieldParam(1));
    }
    else {
      m_BendingPoint = (0.5-EFieldParam(2))*Thickness();
      m_EFieldMax = 2.0*BiasVoltage()/((EFieldParam(1)-EFieldParam(0))*m_BendingPoint+0.5*(2.0+EFieldParam(0)+EFieldParam(1))*Thickness());
      m_Alpha1 = (m_BendingPoint+0.5*Thickness())/(1.-EFieldParam(1));
      m_Alpha2 = (m_BendingPoint-0.5*Thickness())/(1.-EFieldParam(0));
    }

    m_EFieldMean = m_BiasVoltage/m_Thickness; 
    m_Alpha = 0.;
  }
  else {
    std::cout << "Error: E field mode is invalid. " << std::endl;
    return false;
  }
  return true;
}


bool EFieldModel::setEField(double bias, int mode, double p0, double p1, double p2, double /* p3 */)
{
  if (mode==1) {
    return setEField(bias, mode);
  }
  else if (mode==2) {
    return setEField(bias, mode, p0);
  }
  else if (mode==3) {
    return setEField(bias, mode, p0, p1, p2);
  }

  std::cout << "Error: E field mode is invalid. " << std::endl;
  return false;
}


double EFieldModel::DriftTimeMobility(double z, double zInitial) const
{
  double muT = 0.0;
  if (EFieldMode()==1) {
    muT = TMath::Abs((z-zInitial)/m_EFieldMean);
  }
  else if (EFieldMode()==2) {
    muT = TMath::Abs(m_Alpha*TMath::Log((z+m_Alpha)/(zInitial+m_Alpha))/m_EFieldMean);
  }
  else if (EFieldMode()==3) {
    if (zInitial <= z) {
      if (m_BendingPoint <= zInitial) {
        muT = TMath::Abs(m_Alpha2*TMath::Log((z-m_BendingPoint+m_Alpha2)/(zInitial-m_BendingPoint+m_Alpha2)))/m_EFieldMax;
      }
      else {
        if (z <= m_BendingPoint) {
          muT = TMath::Abs(m_Alpha1*TMath::Log((z-m_BendingPoint+m_Alpha1)/(zInitial-m_BendingPoint+m_Alpha1)))/m_EFieldMax;
        }
        else {
          muT = (TMath::Abs(m_Alpha1*TMath::Log(m_Alpha1/(zInitial-m_BendingPoint+m_Alpha1))) + TMath::Abs(m_Alpha2*TMath::Log((z-m_BendingPoint+m_Alpha2)/m_Alpha2)))/m_EFieldMax;
        }
      }
    }
    else {
      if (m_BendingPoint >= zInitial) {
        muT = TMath::Abs(m_Alpha1*TMath::Log((z-m_BendingPoint+m_Alpha1)/(zInitial-m_BendingPoint+m_Alpha1)))/m_EFieldMax;
      }
      else {
        if (z >= m_BendingPoint) {
          muT = TMath::Abs(m_Alpha2*TMath::Log((z-m_BendingPoint+m_Alpha2)/(zInitial-m_BendingPoint+m_Alpha2)))/m_EFieldMax;
        }
        else {
          muT = (TMath::Abs(m_Alpha2*TMath::Log(m_Alpha2/(zInitial-m_BendingPoint+m_Alpha2))) + TMath::Abs(m_Alpha1*TMath::Log((z-m_BendingPoint+m_Alpha1)/m_Alpha1)))/m_EFieldMax;
        }
      }
    }
  }

  return muT;
}

}
