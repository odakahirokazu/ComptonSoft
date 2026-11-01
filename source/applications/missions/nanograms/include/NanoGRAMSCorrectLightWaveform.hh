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

#ifndef COMPTONSOFT_NanoGRAMSCorrectLightWaveform_H
#define COMPTONSOFT_NanoGRAMSCorrectLightWaveform_H 1

#include "VCSModule.hh"

namespace comptonsoft {

namespace grams {
struct Config;
}

/**
 * Corrections of the light waveforms (LightData::Waveform, in voltage) of NanoGRAMS detector units.
 * The corrections enabled in the light section of the configuration are applied in this order:
 *   1. pedestal (light.pedestal_correction), estimated by light.pedestal_method:
 *      value_range (samples whose values are in [pedestal_range_min, pedestal_range_max] in ADC) or
 *      time_window (samples in light.pedestal_time_window_us, time = 0 at the trigger)
 *   2. digitizer offset of the interleaved ADC phases (light.digitizer_offset_correction)
 *   3. FFT band-pass filter (light.fft_filter)
 * The configuration is taken from NanoGRAMSLoadConfig.
 * This module should be placed between the reader (NanoGRAMSReadTPCEvents or NanoGRAMSReadDppEvents)
 * and NanoGRAMSAnalyzeLight.
 *
 * @date 2026-09-24 | ported from lightVoltageWaveformFromViewImpl of feature/pipeline_nanograms
 */
class NanoGRAMSCorrectLightWaveform : public VCSModule
{
  DEFINE_ANL_MODULE(NanoGRAMSCorrectLightWaveform, 1.0);

public:
  NanoGRAMSCorrectLightWaveform();
  ~NanoGRAMSCorrectLightWaveform() override;

  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  const grams::Config* config_ = nullptr;
  bool warnedPedestalWindow_ = false;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NanoGRAMSCorrectLightWaveform_H */
