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

// DeviceSimulation class
// DeviceSimulation.cc
//
// 2007-12-xx Hirokazu Odaka
// 2008-07-29 Hirokazu Odaka
// 2008-08-25 Hirokazu Odaka
// 2008-11-11 Hirokazu Odaka
// 2009-05-20 Hirokazu Odaka
// 2009-06-01 Hirokazu Odaka
// 2009-07-09 Hirokazu Odaka

#include "DeviceSimulation.hh"

#include "globals.hh"
#include "TSpline.h"
#include "DetectorHit.hh"
#include "HXISGDFlagDef.hh"


namespace comptonsoft {

DeviceSimulation::DeviceSimulation()
  : m_SimPHAMode(0),
    m_NumPixelInWPCalculation(5.0),
    m_SimDiffusionMode(0),
    m_DiffusionDivisionNumber(64),
    m_DiffusionSigmaConstantAnode(0.),
    m_DiffusionSigmaConstantCathode(0.),
    m_DiffusionSpreadFactorAnode(1.),
    m_DiffusionSpreadFactorCathode(1.)
{
  m_EField = new EFieldModel;

  m_GainCorrectionFunction.clear();
  m_GainCorrectionFactor.clear();
}


DeviceSimulation::~DeviceSimulation()
{
  m_GainCorrectionFunction.clear();
  m_GainCorrectionFactor.clear();

  delete m_EField;
}


void DeviceSimulation::initializeTable()
{
  m_GainCorrectionFunction.assign(SizeOfTable(), static_cast<const TSpline*>(0));
  m_GainCorrectionFactor.assign(SizeOfTable(), 1.0);

  BasicDeviceSimulation::initializeTable();
}


double DeviceSimulation::calculatePHA(const StripPair& sp, double edep, double x, double y, double z)
{
  double pha = edep;

  if (SimPHAMode() == 0) {;}
  else if (SimPHAMode() == 1) {
    pha *= ChargeCollectionEfficiencyByHecht(z);
  }
  else if (SimPHAMode() >= 2) {
    pha *= ChargeCollectionEfficiency(sp, x, y ,z);
  }

  return pha;
}


double DeviceSimulation::DiffusionSigmaAnode(double z)
{
  // electron diffusion
  double sigma = 0.;
  if (SimDiffusionMode()==0) {
    ;
  }
  else if (SimDiffusionMode()==1) {
    const double Temperature = 260.0 * kelvin;
    const double KTOverQe = ( k_Boltzmann * Temperature ) / eplus;
    
    double zAnode;
    if (UpsideAnode()) {
      zAnode = 0.5 * DetectorThickness();
    }
    else {
      zAnode = -0.5 * DetectorThickness();
    }
    double mut = m_EField->DriftTimeMobility(zAnode, z);
    sigma = std::sqrt(2. * KTOverQe * mut) * DiffusionSpreadFactorAnode();
  }
  else if (SimDiffusionMode()==2) {
    sigma = DiffusionSigmaConstantAnode();
  }

  return sigma;
}


double DeviceSimulation::DiffusionSigmaCathode(double z)
{
  // hole diffusion
  double sigma = 0.;

  if (SimDiffusionMode()==0) {
    ;
  }
  else if (SimDiffusionMode()==1) {
    const double Temperature = 260.0 * kelvin;
    const double KTOverQe = ( k_Boltzmann * Temperature ) / eplus;
    
    double zCathode;
    if (UpsideAnode()) {
      zCathode = -0.5 * DetectorThickness();
    }
    else {
      zCathode = 0.5 * DetectorThickness();
    }
    double mut = m_EField->DriftTimeMobility(zCathode, z);
    sigma = std::sqrt(2. * KTOverQe * mut) * DiffusionSpreadFactorCathode();
  }
  else if (SimDiffusionMode()==2) {
    sigma = DiffusionSigmaConstantCathode();
  }

  return sigma;
}


void DeviceSimulation::setGainCorrectionFunctionAll(const TSpline* val)
{
  int n = m_GainCorrectionFunction.size();
  for (int i=0; i<n; i++ ) {
    m_GainCorrectionFunction[i] = val;
  }
}


void DeviceSimulation::setGainCorrectionFactorAll(double val)
{
  int n = m_GainCorrectionFactor.size();
  for (int i=0; i<n; i++ ) {
    m_GainCorrectionFactor[i] = val;
  }
}


double DeviceSimulation::correctGain(const StripPair& sp, double ePI)
{
  double factor = 1.0;
  const TSpline* correction = GainCorrectionFunction(sp);
  if (correction) {
    factor = correction->Eval(ePI);
  }
  else {
    factor = GainCorrectionFactor(sp);
  }
  return factor * ePI;
}


double DeviceSimulation::calculatePI(double pha, const StripPair& sp)
{
  double ePI = BasicDeviceSimulation::calculatePI(pha, sp);
  return correctGain(sp, ePI);
}


void DeviceSimulation::printSimParam()
{
  std::cout << "Sim PHA mode : " << SimPHAMode() << '\n'
            << " upside anode   : " << UpsideAnode() << '\n'
            << " thickness      : " << m_EField->Thickness()/mm << " mm" << '\n';
  if (CCEMapName()=="") {
    std::cout << " Bias voltage   : " << BiasVoltage()/volt << " V\n"
              << " E field mode   : " << EFieldMode() << '\n'
              << " E field param0 : " << EFieldParam(0) << '\n'
              << " E field param1 : " << EFieldParam(1) << '\n'
              << " E field param2 : " << EFieldParam(2) << '\n'
              << " E field param3 : " << EFieldParam(3) << '\n'
              << " mutau e        : " << MuTauElectron()/(cm2/volt) << " cm2/V\n"
              << " mutau h        : " << MuTauHole()/(cm2/volt) << " cm2/V\n"
              << " num pixel for WP calculationE : " << NumPixelInWPCalculation() << '\n';
  }
  else {
    std::cout << " CCE map name   : " << CCEMapName() << "\n";
  }
  
  std::cout << '\n'
            << "Sim Diffusion mode : " << SimDiffusionMode() << '\n'
            << " Constant Anode        : " << DiffusionSigmaConstantAnode()/um << " um\n"
            << " Constant Cathode      : " << DiffusionSigmaConstantCathode()/um << " um\n"
            << " Spread factor Anode   : " << DiffusionSpreadFactorAnode() << '\n'
            << " Spread factor Cathode : " << DiffusionSpreadFactorCathode() << '\n'
            << '\n'
            << "Quenching factor : " << QuenchingFactor() << std::endl;
}

}
