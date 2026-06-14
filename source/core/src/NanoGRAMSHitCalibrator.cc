/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                    *
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

#include "NanoGRAMSHitCalibrator.hh"

#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <stdexcept>

#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "NanoGRAMSTPCDataProcessor.hh"
#include "NanoGRAMSTPCProperty.hh"

namespace comptonsoft
{

namespace unit = anlgeant4::unit;

namespace
{

constexpr double kPixelSize = 0.32 * unit::cm;

using PositionTable   = std::array<std::array<double, NUM_CH_EACH_VATA>, NUM_VATA>;
using PixelIndexTable = std::array<int16_t, NUM_CH_EACH_VATA>;

constexpr PositionTable buildPosX()
{
  PositionTable dict{};
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      if (fec == 0) {
        constexpr double offset_x = -1.28 * unit::cm;
        dict[fec][ch] = offset_x + kPixelSize * (ch / 8 - 3.5);
      } else if (fec == 1) {
        constexpr double offset_x = 1.28 * unit::cm;
        if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
          dict[fec][ch] = offset_x + kPixelSize * ((ch % 8) - 3.5);
        } else {
          dict[fec][ch] = offset_x + kPixelSize * (3.5 - (ch % 8));
        }
      } else if (fec == 2) {
        constexpr double offset_x = 1.28 * unit::cm;
        dict[fec][ch] = offset_x + kPixelSize * (3.5 - ch / 8);
      } else {
        constexpr double offset_x = -1.28 * unit::cm;
        if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
          dict[fec][ch] = offset_x + kPixelSize * (3.5 - (ch % 8));
        } else {
          dict[fec][ch] = offset_x + kPixelSize * ((ch % 8) - 3.5);
        }
      }
    }
  }
  return dict;
}

constexpr PositionTable buildPosY()
{
  PositionTable dict{};
  for (int fec = 0; fec < NUM_VATA; ++fec) {
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      if (fec == 0) {
        constexpr double offset_y = -1.28 * unit::cm;
        if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
          dict[fec][ch] = offset_y + kPixelSize * (3.5 - (ch % 8));
        } else {
          dict[fec][ch] = offset_y + kPixelSize * ((ch % 8) - 3.5);
        }
      } else if (fec == 1) {
        constexpr double offset_y = -1.28 * unit::cm;
        dict[fec][ch] = offset_y + kPixelSize * (ch / 8 - 3.5);
      } else if (fec == 2) {
        constexpr double offset_y = 1.28 * unit::cm;
        if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
          dict[fec][ch] = offset_y + kPixelSize * ((ch % 8) - 3.5);
        } else {
          dict[fec][ch] = offset_y + kPixelSize * (3.5 - (ch % 8));
        }
      } else {
        constexpr double offset_y = 1.28 * unit::cm;
        dict[fec][ch] = offset_y + kPixelSize * (3.5 - ch / 8);
      }
    }
  }
  return dict;
}

constexpr PixelIndexTable buildPixelX()
{
  PixelIndexTable dict{};
  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    dict[ch] = static_cast<int16_t>(ch / 8);
  }
  return dict;
}

constexpr PixelIndexTable buildPixelY()
{
  PixelIndexTable dict{};
  for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
    if ((ch < 16) || ((23 < ch) && (ch < 32)) || ((39 < ch) && (ch < 48))) {
      dict[ch] = static_cast<int16_t>(7 - (ch % 8));
    } else {
      dict[ch] = static_cast<int16_t>(ch % 8);
    }
  }
  return dict;
}

const PositionTable kPosX     = buildPosX();
const PositionTable kPosY     = buildPosY();
const PixelIndexTable kPixelX = buildPixelX();
const PixelIndexTable kPixelY = buildPixelY();

} // namespace

std::vector<DetectorHit_sptr> buildCalibratedHits(
    const CalibrationConfig& config,
    const TPCProperty& tpc_property,
    const std::vector<grams::RawFECHit>& raw_hits)
{
  std::vector<DetectorHit_sptr> hits;
  hits.reserve(raw_hits.size());

  const double max_time = config.energy.max_time;

  for (const auto& raw_hit : raw_hits) {
    if (raw_hit.fec < 0 || raw_hit.fec >= NUM_VATA) {
      throw std::runtime_error("FEC index out of range in NanoGRAMS calibration.");
    }

    const std::size_t n = std::min(raw_hit.channels.size(), raw_hit.adus.size());
    if (n == 0) {
      continue;
    }

    double total_energy   = 0.0 * unit::keV;
    double posx           = 0.0 * unit::cm;
    double posy           = 0.0 * unit::cm;
    double posz           = 0.0 * unit::cm;
    std::size_t max_index = 0;
    double max_energy     = -std::numeric_limits<double>::infinity() * unit::keV;

    std::vector<double> energies(n, 0.0);
    std::vector<int> channel_fecs(n, raw_hit.fec);
    for (std::size_t i = 0; i < n; ++i) {
      int fec = raw_hit.fec;
      if (i < raw_hit.channel_fecs.size()) {
        fec = raw_hit.channel_fecs[i];
      }
      const int ch = raw_hit.channels[i];
      if (fec < 0 || fec >= NUM_VATA) {
        throw std::runtime_error("FEC index out of range in NanoGRAMS calibration.");
      }
      if (ch < 0 || ch >= NUM_CH_EACH_VATA) {
        throw std::runtime_error("Channel index out of range in NanoGRAMS calibration.");
      }

      const double corrected_adu =
          static_cast<double>(raw_hit.adus[i]) *
          tpc_property.temperatureCorrectionFactor(fec);
      const double energy =
          tpc_property.convertADC2keVWithSpline3D(fec, ch, corrected_adu);
      energies[i]   = energy;
      channel_fecs[i] = fec;
      total_energy += energy;

      if (energy > max_energy) {
        max_energy = energy;
        max_index  = i;
      }
    }

    if (!(total_energy > 0.0)) {
      continue;
    }

    for (std::size_t i = 0; i < n; ++i) {
      const int ch = raw_hit.channels[i];
      const int fec = channel_fecs[i];
      const double weight = energies[i] / total_energy;
      posx += weight * kPosX[fec][ch];
      posy += weight * kPosY[fec][ch];
      posz += weight *
              tpc_property.convertDriftTime2PosZScale(raw_hit.drift_time, max_time);
    }

    const int channel = raw_hit.channels[max_index];
    const int channel_fec = channel_fecs[max_index];
    auto hit = std::make_shared<DetectorHit>();
    hit->setTI(static_cast<int64_t>(raw_hit.ti));
    hit->setDetectorChannelID(ChannelID::Undefined, channel_fec, channel);

    if ((-2.56 * unit::cm < posx) && (posx < 2.56 * unit::cm) &&
        (-2.56 * unit::cm < posy) && (posy < 2.56 * unit::cm)) {
      if (posx > 0.0 * unit::cm) {
        if (posy > 0.0 * unit::cm) {
          hit->setDetectorID(2);
        } else {
          hit->setDetectorID(1);
        }
      } else {
        if (posy > 0.0 * unit::cm) {
          hit->setDetectorID(3);
        } else {
          hit->setDetectorID(0);
        }
      }
    } else {
      hit->setDetectorID(-1);
    }

    hit->setReadoutChannelID(channel_fec, channel_fec, channel);
    hit->setVoxel(kPixelX[channel], kPixelY[channel], VoxelID::Undefined);
    hit->setEnergy(total_energy);
    hit->setPosition(posx, posy, posz);
    hit->setPositionError(tpc_property.posXError(),
                          tpc_property.posYError(),
                          tpc_property.posZError());
    hits.push_back(std::move(hit));
  }

  return hits;
}

} /* namespace comptonsoft */
