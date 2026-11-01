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

#include "NanoGRAMSCorrectLightWaveform.hh"

#include <cmath>
#include <iostream>
#include <vector>

#include "AstroUnits.hh"
#include "LightData.hh"
#include "NanoGRAMSConfig.hh"
#include "NanoGRAMSLightWaveformCorrection.hh"
#include "NanoGRAMSLoadConfig.hh"
#include "RealDetectorUnitNanoGRAMS.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

NanoGRAMSCorrectLightWaveform::NanoGRAMSCorrectLightWaveform() = default;

NanoGRAMSCorrectLightWaveform::~NanoGRAMSCorrectLightWaveform() = default;

ANLStatus NanoGRAMSCorrectLightWaveform::mod_initialize()
{
  const ANLStatus status = VCSModule::mod_initialize();
  if (status != AS_OK) {
    return status;
  }

  const NanoGRAMSLoadConfig* configLoader = nullptr;
  get_module("NanoGRAMSLoadConfig", &configLoader);
  config_ = &configLoader->config();
  return AS_OK;
}

ANLStatus NanoGRAMSCorrectLightWaveform::mod_analyze()
{
  const grams::Config& cfg = *config_;
  // the pedestal range is given in ADC; the waveform is in voltage
  const double adcToVoltage = cfg.adc2mv * (unit::volt / 1000.0);

  for (auto& detector : getDetectorManager()->getDetectors()) {
    if (!detector->checkType(DetectorType::NanoGRAMS)) {
      continue;
    }
    // always RealDetectorUnitNanoGRAMS
    auto* nanograms = static_cast<RealDetectorUnitNanoGRAMS*>(detector.get());

    for (int dppChannel = 0; dppChannel < nanograms->NumberOfLightData(); ++dppChannel) {
      LightData* lightData = nanograms->getLightData(dppChannel);
      if (!lightData->isValid()) {
        continue;
      }
      std::vector<double>& waveform = lightData->Waveform();
      // sampling interval in ns (= wave_compress)
      const double timeWidthNs = lightData->TimeWidth() / unit::ns;

      if (cfg.light_pedestal_correction) {
        if (cfg.light_pedestal_method == grams::LightPedestalMethod::TimeWindow) {
          // mean of the samples in the time window (time = 0 at the trigger)
          const LightData::range_t window(cfg.light_pedestal_time_window_start, cfg.light_pedestal_time_window_stop);
          const double pedestal = lightData->mean(window);
          if (std::isfinite(pedestal)) {
            lightData->setPedestal(pedestal);
            lightData->subtractPedestal();
          }
          else if (!warnedPedestalWindow_) {
            std::cout << "[NanoGRAMSCorrectLightWaveform] WARNING: the pedestal time window has no samples "
                      << "(DPP ch " << dppChannel << ", waveform starts at " << lightData->TimeStart() / unit::us
                      << " us); the pedestal is not subtracted." << std::endl;
            warnedPedestalWindow_ = true;
          }
        }
        else {
          const grams::PedestalCorrectionResult result = grams::correctPedestal(
              waveform, cfg.light_pedestal_range_min * adcToVoltage, cfg.light_pedestal_range_max * adcToVoltage);
          lightData->setPedestal(result.pedestal);
        }
      }

      if (cfg.light_digitizer_offset_correction) {
        grams::correctDigitizerOffset(waveform, static_cast<int>(std::lround(timeWidthNs)),
                                      cfg.light_digitizer_offset_range_start_index,
                                      cfg.light_digitizer_offset_range_stop_index);
      }

      if (cfg.light_fft_filter) {
        grams::applySimpleFFTFilter(waveform, timeWidthNs, cfg.light_fft_low_frequency, cfg.light_fft_high_frequency);
      }
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
