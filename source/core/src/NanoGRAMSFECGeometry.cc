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

#include "NanoGRAMSFECGeometry.hh"

#include "NanoGRAMSConfig.hh"

#include <algorithm>

namespace comptonsoft
{
namespace grams
{
namespace
{

std::pair<int, int> sectionOrigin(int fec)
{
  if (fec == 0) {
    return {0, 0};
  }
  if (fec == 1) {
    return {kFECSectionSidePixels, 0};
  }
  if (fec == 2) {
    return {kFECSectionSidePixels, kFECSectionSidePixels};
  }
  return {0, kFECSectionSidePixels};
}

} // namespace

FECChannelGeometry buildFECChannelGeometry()
{
  FECChannelGeometry geom;
  std::array<std::array<std::pair<int, int>, kTPCPlaneSidePixels>,
             kTPCPlaneSidePixels> global_to_fec_ch{};
  for (auto& col : global_to_fec_ch) {
    col.fill({-1, -1});
  }

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const auto& disp_to_ch = kPlotNumAll[fec];
    const auto [origin_x, origin_y] = sectionOrigin(fec);

    for (int xx = 0; xx < kFECSectionSidePixels; ++xx) {
      for (int yy = 0; yy < kFECSectionSidePixels; ++yy) {
        const int ch = disp_to_ch[xx * kFECSectionSidePixels + yy];
        geom.xy_of_ch[fec][ch] = {xx, yy};
        geom.global_xy_of_ch[fec][ch] = {origin_x + xx, origin_y + yy};
        global_to_fec_ch[origin_x + xx][origin_y + yy] = {fec, ch};
      }
    }

    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      const auto [x, y] = geom.xy_of_ch[fec][ch];

      for (const auto& delta : {std::pair<int, int>{-1, 0},
                                std::pair<int, int>{1, 0},
                                std::pair<int, int>{0, -1},
                                std::pair<int, int>{0, 1}}) {
        const int xx = x + delta.first;
        const int yy = y + delta.second;
        if (0 <= xx && xx < kFECSectionSidePixels &&
            0 <= yy && yy < kFECSectionSidePixels) {
          geom.cross_neighbors[fec][ch].push_back(
              disp_to_ch[xx * kFECSectionSidePixels + yy]);
        }
      }

      for (const auto& delta : {std::pair<int, int>{-1, -1},
                                std::pair<int, int>{-1, 1},
                                std::pair<int, int>{1, -1},
                                std::pair<int, int>{1, 1}}) {
        const int xx = x + delta.first;
        const int yy = y + delta.second;
        if (0 <= xx && xx < kFECSectionSidePixels &&
            0 <= yy && yy < kFECSectionSidePixels) {
          geom.diag_neighbors[fec][ch].push_back(
              disp_to_ch[xx * kFECSectionSidePixels + yy]);
        }
      }
    }

    std::vector<int> periphery;
    for (int xx = 0; xx < kFECSectionSidePixels; ++xx) {
      for (int yy = 0; yy < kFECSectionSidePixels; ++yy) {
        if (xx == 0 || xx == kFECSectionSidePixels - 1 ||
            yy == 0 || yy == kFECSectionSidePixels - 1) {
          periphery.push_back(disp_to_ch[xx * kFECSectionSidePixels + yy]);
        }
      }
    }

    std::sort(periphery.begin(), periphery.end());
    periphery.erase(std::unique(periphery.begin(), periphery.end()), periphery.end());
    geom.periphery[fec] = std::move(periphery);
  }

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      const auto [global_x, global_y] = geom.global_xy_of_ch[fec][ch];

      for (const auto& delta : {std::pair<int, int>{-1, 0},
                                std::pair<int, int>{1, 0},
                                std::pair<int, int>{0, -1},
                                std::pair<int, int>{0, 1}}) {
        const int xx = global_x + delta.first;
        const int yy = global_y + delta.second;
        if (0 <= xx && xx < kTPCPlaneSidePixels &&
            0 <= yy && yy < kTPCPlaneSidePixels) {
          const auto [other_fec, other_ch] = global_to_fec_ch[xx][yy];
          if (other_fec >= 0 && other_fec != fec) {
            geom.cross_section_neighbors[fec][ch].push_back({other_fec, other_ch});
          }
        }
      }

      for (const auto& delta : {std::pair<int, int>{-1, -1},
                                std::pair<int, int>{-1, 1},
                                std::pair<int, int>{1, -1},
                                std::pair<int, int>{1, 1}}) {
        const int xx = global_x + delta.first;
        const int yy = global_y + delta.second;
        if (0 <= xx && xx < kTPCPlaneSidePixels &&
            0 <= yy && yy < kTPCPlaneSidePixels) {
          const auto [other_fec, other_ch] = global_to_fec_ch[xx][yy];
          if (other_fec >= 0 && other_fec != fec) {
            geom.diag_section_neighbors[fec][ch].push_back({other_fec, other_ch});
          }
        }
      }
    }
  }

  return geom;
}

PixelMask buildFECMask(const Config& cfg, int fec)
{
  PixelMask mask{};
  mask.fill(1);

  const auto it = cfg.exclude_pix.find(fec);
  if (it != cfg.exclude_pix.end()) {
    for (int ch : it->second) {
      if (0 <= ch && ch < NUM_CH_EACH_VATA) {
        mask[ch] = 0;
      }
    }
  }
  return mask;
}

} /* namespace grams */
} /* namespace comptonsoft */
