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

namespace unit = anlgeant4::unit;

namespace comptonsoft {

DeviceSimulation::DeviceSimulation()
  : chargeCollectionMode_(0),
    CCEMapName_(""),
    EField_(new EFieldModel),
    numPixelsInWPCalculation_(5.0),
    diffusionMode_(0),
    diffusionDivisionNumber_(64),
    diffusionSigmaConstantAnode_(0.0),
    diffusionSigmaConstantCathode_(0.0),
    diffusionSpreadFactorAnode_(1.0),
    diffusionSpreadFactorCathode_(1.0),
    temperature_(260.0*unit::kelvin)
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

  if (ChargeCollectionMode() == 0) {;}
  else if (ChargeCollectionMode() == 1) {
    energyCharge *= ChargeCollectionEfficiencyByHecht(z);
  }
  else if (ChargeCollectionMode() >= 2) {
    energyCharge *= ChargeCollectionEfficiency(pixel, x, y ,z);
  }

  return energyCharge;
}

double DeviceSimulation::DiffusionSigmaAnode(double z)
{
  // electron diffusion
  double sigma = 0.;
  if (DiffusionMode()==0) {
    ;
  }
  else if (DiffusionMode()==1) {
    const double KTOverQe = ( CLHEP::k_Boltzmann * Temperature() ) / CLHEP::eplus;
    
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
  else if (DiffusionMode()==2) {
    sigma = DiffusionSigmaConstantAnode();
  }

  return sigma;
}

double DeviceSimulation::DiffusionSigmaCathode(double z)
{
  // hole diffusion
  double sigma = 0.;

  if (DiffusionMode()==0) {
    ;
  }
  else if (DiffusionMode()==1) {
    const double KTOverQe = ( CLHEP::k_Boltzmann * Temperature() ) / CLHEP::eplus;
    
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
  else if (DiffusionMode()==2) {
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
  const double EPI = VDeviceSimulation::calculateEPI(energyCharge, pixel);
  const double EPICompensated = compensateEPI(pixel, EPI);
  return EPICompensated;
}

void DeviceSimulation::printSimulationParameters(std::ostream& os) const
{
  os << "Quenching factor : " << QuenchingFactor() << '\n';
  os << "Temperature : " << Temperature()/unit::kelvin << " K\n";
  os << "E-field\n"
     << "  Bias voltage   : " << BiasVoltage()/unit::volt << " V\n"
     << "  E field mode   : " << static_cast<int>(EFieldMode()) << '\n'
     << "  E field param0 : " << EFieldParam(0) << '\n'
     << "  E field param1 : " << EFieldParam(1) << '\n'
     << "  E field param2 : " << EFieldParam(2) << '\n'
     << "  E field param3 : " << EFieldParam(3) << '\n'
     << "  Thickness      : " << EField_->Thickness()/unit::cm << " cm" << '\n';
  os << "Charge collection\n"
     << "  Mode: " << ChargeCollectionMode() << '\n';
  if (CCEMapName()=="") {
    os << "  mutau e : " << MuTauElectron()/(unit::cm2/unit::volt) << " cm2/V\n"
       << "  mutau h : " << MuTauHole()/(unit::cm2/unit::volt) << " cm2/V\n"
       << "  number of pixels for WP calculation : " << NumPixelsInWPCalculation() << '\n';
  }
  else {
    os << "  CCE map name : " << CCEMapName() << "\n";
  }
  os << "Diffusion\n"
     << "  Mode: " << DiffusionMode() << '\n'
     << "  Spread factor Anode   : " << DiffusionSpreadFactorAnode() << '\n'
     << "  Spread factor Cathode : " << DiffusionSpreadFactorCathode() << '\n'
     << "  Constant Anode        : " << DiffusionSigmaConstantAnode()/unit::um << " um\n"
     << "  Constant Cathode      : " << DiffusionSigmaConstantCathode()/unit::um << " um\n";
  os << "Timing resolution\n"
     << "  for trigger            : " << TimingResolutionForTrigger()/unit::ns << " ns\n"
     << "  for energy measurement : " << TimingResolutionForEnergyMeasurement()/unit::ns << " ns\n";
  os << "Pedestal generation : " << isPedestalEnabled() << '\n';
}

} /* nemespace comptonsoft */
