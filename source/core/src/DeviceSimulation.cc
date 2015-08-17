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

#include "DeviceSimulation.hh"
#include "AstroUnits.hh"
#include "TSpline.h"
#include "DetectorHit.hh"
#include "FlagDefinition.hh"

namespace comptonsoft {

DeviceSimulation::DeviceSimulation()
  : simPHAMode_(0),
    CCEMapName_("cce"),
    EField_(new EFieldModel),
    numPixelsInWPCalculation_(5.0),
    simDiffusionMode_(0),
    diffusionDivisionNumber_(64),
    diffusionSigmaConstantAnode_(0.0),
    diffusionSigmaConstantCathode_(0.0),
    diffusionSpreadFactorAnode_(1.0),
    diffusionSpreadFactorCathode_(1.0),
    temperature_(260.0*kelvin)
{
}

DeviceSimulation::~DeviceSimulation() = default;

void DeviceSimulation::initializeTables()
{
  VDeviceSimulation::initializeTables();
  EPICompensationFunctionVector_.assign(SizeOfTable(), static_cast<const TSpline*>(0));
  EPICompensationFactorVector_.assign(SizeOfTable(), 1.0);
}

double DeviceSimulation::
calculateEnergyCharge(const PixelID& pixel,
                      double energyDeposit,
                      double x, double y, double z) const
{
  double energyCharge = energyDeposit;

  if (SimPHAMode() == 0) {;}
  else if (SimPHAMode() == 1) {
    energyCharge *= ChargeCollectionEfficiencyByHecht(z);
  }
  else if (SimPHAMode() >= 2) {
    energyCharge *= ChargeCollectionEfficiency(pixel, x, y ,z);
  }

  return energyCharge;
}

double DeviceSimulation::DiffusionSigmaAnode(double z)
{
  // electron diffusion
  double sigma = 0.;
  if (SimDiffusionMode()==0) {
    ;
  }
  else if (SimDiffusionMode()==1) {
    const double KTOverQe = ( k_Boltzmann * Temperature() ) / eplus;
    
    double zAnode;
    if (isUpSideAnode()) {
      zAnode = 0.5 * getThickness();
    }
    else {
      zAnode = -0.5 * getThickness();
    }
    double mut = EField_->DriftTimeMobility(zAnode, z);
    sigma = std::sqrt(2.0 * KTOverQe * mut) * DiffusionSpreadFactorAnode();
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
    const double KTOverQe = ( k_Boltzmann * Temperature() ) / eplus;
    
    double zCathode;
    if (isUpSideAnode()) {
      zCathode = -0.5 * getThickness();
    }
    else {
      zCathode = 0.5 * getThickness();
    }
    double mut = EField_->DriftTimeMobility(zCathode, z);
    sigma = std::sqrt(2. * KTOverQe * mut) * DiffusionSpreadFactorCathode();
  }
  else if (SimDiffusionMode()==2) {
    sigma = DiffusionSigmaConstantCathode();
  }

  return sigma;
}

double DeviceSimulation::
compensateEPI(const PixelID& sp, double ePI) const
{
  double factor = 1.0;
  const TSpline* function = getEPICompensationFunction(sp);
  if (function) {
    factor = function->Eval(ePI);
  }
  else {
    factor = getEPICompensationFactor(sp);
  }
  return factor * ePI;
}

double DeviceSimulation::
calculateEPI(double energyCharge, const PixelID& pixel) const
{
  double ePI = VDeviceSimulation::calculateEPI(energyCharge, pixel);
  return compensateEPI(pixel, ePI);
}

void DeviceSimulation::printSimulationParameters()
{
  std::cout << "Sim PHA mode : " << SimPHAMode() << '\n'
            << " upside anode   : " << isUpSideAnode() << '\n'
            << " thickness      : " << EField_->Thickness()/mm << " mm" << '\n';
  if (CCEMapName()=="") {
    std::cout << " Bias voltage   : " << BiasVoltage()/volt << " V\n"
              << " E field mode   : " << static_cast<int>(EFieldMode()) << '\n'
              << " E field param0 : " << EFieldParam(0) << '\n'
              << " E field param1 : " << EFieldParam(1) << '\n'
              << " E field param2 : " << EFieldParam(2) << '\n'
              << " E field param3 : " << EFieldParam(3) << '\n'
              << " mutau e        : " << MuTauElectron()/(cm2/volt) << " cm2/V\n"
              << " mutau h        : " << MuTauHole()/(cm2/volt) << " cm2/V\n"
              << " num pixel for WP calculationE : " << NumPixelsInWPCalculation() << '\n';
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
            << " Device temperature: " << Temperature()/kelvin << " K\n"
            << '\n'
            << "Quenching factor : " << QuenchingFactor() << std::endl;
}

} /* nemespace comptonsoft */
