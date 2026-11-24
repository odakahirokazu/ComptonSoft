#include "NanoGRAMSLoadConfig.hh"
#include "GainFunctionCubic.hh"
#include "LightData.hh"
#include "NanoGRAMSConstants.hh"
#include "NanoGRAMSMultiChannelData.hh"
#include "NanoGRAMSTemperatureCorrection.hh"
#include "RealDetectorUnitNanoGRAMS.hh"
#include "boost/format.hpp"
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>

using namespace anlnext;
namespace comptonsoft {
ANLStatus NanoGRAMSLoadConfig::mod_define()
{
  define_parameter("config_file", &mod_class::config_file_);
  define_parameter("dpp_config_file", &mod_class::dpp_config_file_);
  define_parameter("gain_tp_file", &mod_class::gain_tp_file_);
  define_parameter("gain_tp_hash", &mod_class::gain_tp_dict_);
  define_map_key("fec", "0");
  add_value_element("gain", &mod_class::gain_tp_value_);
  return AS_OK;
}
ANLStatus NanoGRAMSLoadConfig::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  // settings common to all the input files
  grams::readConfig(cfg_, config_file_);
  if (IsDppConfigSet()) {
    grams::readDPPConfigFile(cfg_, dpp_config_file_);
  }
  setupCalibration();

  return AS_OK;
}

void NanoGRAMSLoadConfig::setupCalibration()
{
  calibration_config_ = readCalibrationConfig(config_file_);
  const std::filesystem::path gain_info_path =
      resolveCalibrationPath(calibration_config_.config_dir, calibration_config_.energy.gain_info_file);

  const EnergyCalibrationConfig& energy = calibration_config_.energy;

  // ADC2C: gain functions of the MCDs; ccal2ADC: test-pulse response at the gain measurement
  std::array<GainMatrix, NUM_VATA> adc2c{};
  std::array<double, NUM_VATA> ccal2adcReference{};
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    adc2c[fec] = loadGainMatrix(gain_info_path, (boost::format("/FEC%1%/ADC2C") % fec).str());
    const GainMatrix ccal2adc = loadGainMatrix(gain_info_path, (boost::format("/FEC%1%/ccal2ADC") % fec).str());
    ccal2adcReference[fec] = evaluateGainCubic(static_cast<double>(energy.ccal), ccal2adc.at(energy.tp_channel));
  }

  if (energy.gain_reference_times && gain_tp_file_.empty()) {
    throw std::runtime_error("gain_reference_time requires gain_tp_file "
                             "(the reference test-pulse ADC is interpolated from its time series).");
  }

  auto temperatureCorrection = std::make_shared<NanoGRAMSTemperatureCorrection>(NUM_VATA);

  // reference test-pulse ADC: ccal2ADC by default, overwritten below if gain_reference_time is given
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    temperatureCorrection->setReferenceADC(fec, ccal2adcReference[fec]);
  }

  if (!gain_tp_file_.empty()) {
    const std::filesystem::path gain_tp_path = resolveCalibrationPath(calibration_config_.config_dir, gain_tp_file_);
    const TestPulseGainTable rows = readTestPulseGainTable(gain_tp_path);
    for (const TestPulseGainRow& row : rows) {
      for (int fec = 0; fec < NUM_VATA; ++fec) {
        if (std::isfinite(row.fec_gain[fec])) {
          temperatureCorrection->addTestPulseADC(fec, row.time, row.fec_gain[fec]);
        }
      }
    }
    std::cout << "[NanoGRAMSLoadConfig] gain_tp_file for temperature correction: " << gain_tp_path << std::endl;

    if (energy.gain_reference_times) {
      std::cout << "[NanoGRAMSLoadConfig] reference test-pulse ADC at gain_reference_time:" << std::endl;
      for (int fec = 0; fec < NUM_VATA; ++fec) {
        const GainReferenceTime& ref = (*energy.gain_reference_times)[fec];
        const double referenceTP = interpolatedTestPulseGains(rows, ref.time)[fec];
        if (!std::isfinite(referenceTP)) {
          throw std::runtime_error((boost::format("gain_reference_time %1% is out of the range of gain_tp_file "
                                                  "for FEC%2%.") %
                                    ref.time_id % fec)
                                       .str());
        }
        temperatureCorrection->setReferenceADC(fec, referenceTP);
        std::cout << "  FEC" << fec << " at " << ref.time_id << ": " << referenceTP
                  << " (ccal2ADC: " << ccal2adcReference[fec]
                  << ", ratio TP/ccal2ADC: " << referenceTP / ccal2adcReference[fec] << ")" << std::endl;
      }
    }
    else {
      std::cout << "[NanoGRAMSLoadConfig] reference test-pulse ADC from ccal2ADC (no gain_reference_time)." << std::endl;
    }
  }
  else if (!gain_tp_dict_.empty()) {
    const std::array<double, NUM_VATA> fixed = fixedTestPulseGainsFromHash(gain_tp_dict_);
    for (int fec = 0; fec < NUM_VATA; ++fec) {
      temperatureCorrection->setFixedTestPulseADC(fec, fixed[fec]);
    }
    std::cout << "[NanoGRAMSLoadConfig] gain_tp_hash for temperature correction." << std::endl;
  }
  else {
    std::cout << "[NanoGRAMSLoadConfig] WARNING: no gain_tp_file/hash. "
              << "Temperature correction factors are 1." << std::endl;
  }

  setupDetectorParameters(adc2c, temperatureCorrection);
}

void NanoGRAMSLoadConfig::setupDetectorParameters(
    const std::array<GainMatrix, NUM_VATA>& adc2c,
    const std::shared_ptr<const NanoGRAMSTemperatureCorrection>& temperatureCorrection)
{
  DetectorSystem* detectorManager = getDetectorManager();
  if (detectorManager == nullptr) {
    return;
  }

  for (auto& detector : detectorManager->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    // always RealDetectorUnitNanoGRAMS
    auto* nanograms = static_cast<RealDetectorUnitNanoGRAMS*>(detector.get());
    nanograms->setMaxDriftTime(calibration_config_.energy.max_time);
    nanograms->setElectricField(calibration_config_.general.efield);
    nanograms->setTemperatureCorrection(temperatureCorrection);

    // cluster selection (the thresholds in the yaml override those in the detector parameters XML;
    // the clustering range is given by the XML). The energies are compared with the charge-equivalent
    // EPI for selection (charge x W_ion).
    nanograms->setClusteringEnergyThreshold(cfg_.core_noise_energy_th);
    nanograms->setClusteringSplitThreshold(cfg_.spread_thr_energy);
    nanograms->setCrossFECMergeDriftTimeTolerance(cfg_.cross_fec_merge_drift_time_tolerance);
    nanograms->setDriftTimeLimit(cfg_.drift_time_max);
    nanograms->setClusterPixelCountRange(cfg_.pix_min, cfg_.pix_max);
    for (const auto& [fec, channels] : cfg_.core_exclude_pix) {
      nanograms->setExcludedCorePixels(fec, channels);
    }

    for (int fec = 0; fec < NUM_VATA; ++fec) {
      NanoGRAMSMultiChannelData* mcd = nanograms->getNanoGRAMSMultiChannelData(fec);
      for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
        // the gain matrix holds cubic parameters from the highest order: p0*x^3 + p1*x^2 + p2*x + p3
        const GainParamArray& p = adc2c[fec][ch];
        mcd->setGainFunction(ch, std::make_shared<GainFunctionCubic>(p[3], p[2], p[1], p[0]));
      }
      // the threshold is compared with the charge-equivalent selection EPI (charge x W_ion)
      mcd->setHitThresholdEnergy(cfg_.spread_thr_energy);
    }
  }
}

} // namespace comptonsoft