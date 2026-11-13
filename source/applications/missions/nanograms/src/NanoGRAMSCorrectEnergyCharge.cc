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

#include "NanoGRAMSCorrectEnergyCharge.hh"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <yaml-cpp/yaml.h>

#include "CSHitCollection.hh"
#include "DetectorHit.hh"
#include "RealDetectorUnitNanoGRAMS.hh"

using namespace anlnext;

namespace comptonsoft {

namespace {

constexpr int NumFECs = RealDetectorUnitNanoGRAMS::NumFECs;
constexpr int SplineRecombinationMode = 4;

std::vector<double> readFactorsFromYAML(const std::string& filename)
{
  const YAML::Node root = YAML::LoadFile(filename);
  const YAML::Node node = root["echarge_correction_factors"];
  if (!node) {
    throw std::runtime_error("NanoGRAMSCorrectEnergyCharge: echarge_correction_factors is not found in " + filename);
  }
  if (!node.IsSequence()) {
    throw std::runtime_error("NanoGRAMSCorrectEnergyCharge: echarge_correction_factors must be an array in " +
                             filename);
  }
  return node.as<std::vector<double>>();
}

} // namespace

NanoGRAMSCorrectEnergyCharge::NanoGRAMSCorrectEnergyCharge() : factors_(NumFECs, 1.0) {}

NanoGRAMSCorrectEnergyCharge::~NanoGRAMSCorrectEnergyCharge() = default;

ANLStatus NanoGRAMSCorrectEnergyCharge::mod_define()
{
  define_parameter("parameter_file", &mod_class::parameterFile_);
  set_parameter_description("yaml file with the key echarge_correction_factors (overrides echarge_correction_factors)");
  define_parameter("echarge_correction_factors", &mod_class::factors_);
  set_parameter_description("factors multiplied to echarge of each FEC (4 values)");
  return AS_OK;
}

ANLStatus NanoGRAMSCorrectEnergyCharge::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }
  get_module_NC("CSHitCollection", &hitCollection_);

  if (!parameterFile_.empty()) {
    factors_ = readFactorsFromYAML(parameterFile_);
  }
  if (static_cast<int>(factors_.size()) != NumFECs) {
    std::ostringstream message;
    message << "NanoGRAMSCorrectEnergyCharge: echarge_correction_factors must contain " << NumFECs << " values (given "
            << factors_.size() << ")";
    throw std::runtime_error(message.str());
  }
  for (double f : factors_) {
    if (!(f > 0.0)) {
      throw std::runtime_error("NanoGRAMSCorrectEnergyCharge: echarge_correction_factors must be positive");
    }
  }

  // the energy is recalculated with the charge-to-energy spline of each detector unit
  // (the electric field is set by NanoGRAMSLoadConfig)
  int numDetectors = 0;
  for (auto& detector : getDetectorManager()->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    const auto* nanograms = static_cast<const RealDetectorUnitNanoGRAMS*>(detector.get());
    if (nanograms->recombinationCorrectionMode() != SplineRecombinationMode) {
      std::ostringstream message;
      message << "NanoGRAMSCorrectEnergyCharge: the recombination correction mode of detector " << nanograms->getID()
              << " is " << nanograms->recombinationCorrectionMode() << " (must be " << SplineRecombinationMode << ")";
      throw std::runtime_error(message.str());
    }
    if (!(nanograms->ElectricField() > 0.0)) {
      throw std::runtime_error("NanoGRAMSCorrectEnergyCharge: the electric field is not set "
                               "(chain NanoGRAMSLoadConfig before this module)");
    }
    ++numDetectors;
  }
  if (numDetectors == 0) {
    throw std::runtime_error("NanoGRAMSCorrectEnergyCharge: no NanoGRAMS detector");
  }

  std::cout << "NanoGRAMSCorrectEnergyCharge: echarge_correction_factors = [";
  for (int fec = 0; fec < NumFECs; ++fec) {
    std::cout << (fec == 0 ? " " : ", ") << factors_[fec];
  }
  std::cout << " ]" << (parameterFile_.empty() ? "" : " (from " + parameterFile_ + ")") << std::endl;

  numHits_ = 0;
  numCorrectedHits_ = 0;
  return AS_OK;
}

ANLStatus NanoGRAMSCorrectEnergyCharge::mod_analyze()
{
  for (int timeGroup = 0; timeGroup < hitCollection_->NumberOfTimeGroups(); ++timeGroup) {
    for (const DetectorHit_sptr& hit : hitCollection_->getHits(timeGroup)) {
      ++numHits_;
      VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(hit->DetectorID());
      if (detector == nullptr || !detector->checkType(DetectorType::NanoGRAMS)) {
        continue;
      }
      const int fec = hit->DetectorSection();
      if (fec < 0 || fec >= NumFECs) {
        std::ostringstream message;
        message << "NanoGRAMSCorrectEnergyCharge: invalid FEC (DetectorSection) " << fec;
        throw std::runtime_error(message.str());
      }
      const double factor = factors_[fec];

      // the stored EPIError is that after the recombination correction: back to the charge-equivalent one
      const double echarge = hit->EnergyCharge();
      const double energy = hit->Energy();
      const double epiErrorCharge = (energy > 0.0) ? hit->EPIError() * echarge / energy : 0.0;

      const double echargeCorrected = echarge * factor;
      hit->setEnergyCharge(echargeCorrected);
      hit->setEPI(echargeCorrected);
      hit->setEPIError(epiErrorCharge * factor);

      // EPI -> EPI, Energy (spline), the same as the data reduction
      const auto* nanograms = static_cast<const RealDetectorUnitNanoGRAMS*>(detector);
      DetectorHitVector single{hit};
      nanograms->applyRecombinationCorrection(single);
      ++numCorrectedHits_;
    }
  }
  return AS_OK;
}

ANLStatus NanoGRAMSCorrectEnergyCharge::mod_end_run()
{
  std::cout << "NanoGRAMSCorrectEnergyCharge: " << numCorrectedHits_ << " / " << numHits_ << " hits corrected"
            << std::endl;
  return AS_OK;
}

} // namespace comptonsoft
