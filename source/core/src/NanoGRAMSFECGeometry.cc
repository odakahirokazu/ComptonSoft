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
#include <array>

namespace comptonsoft
{
namespace grams
{
namespace
{

int channelAtSectionCoordinate(const std::array<int, NUM_CH_EACH_VATA>& grid_to_ch,
                               int x,
                               int y)
{
  const int row_from_top = kFECSectionSidePixels - 1 - y;
  const int col_from_left = x;
  return grid_to_ch[row_from_top * kFECSectionSidePixels + col_from_left];
}

std::pair<int, int> sectionCoordinate(int row_from_top, int col_from_left)
{
  return {col_from_left, kFECSectionSidePixels - 1 - row_from_top};
}

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

AnodeChannelTopology buildAnodeChannelTopology()
{
  AnodeChannelTopology topology;
  std::array<std::array<std::pair<int, int>, kTPCPlaneSidePixels>,
             kTPCPlaneSidePixels> global_to_fec_ch{};
  for (auto& col : global_to_fec_ch) {
    col.fill({-1, -1});
  }

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    const auto& grid_to_ch = kFECSectionGridToChannel[fec];
    const auto [origin_x, origin_y] = sectionOrigin(fec);

    for (int row = 0; row < kFECSectionSidePixels; ++row) {
      for (int col = 0; col < kFECSectionSidePixels; ++col) {
        const int ch = grid_to_ch[row * kFECSectionSidePixels + col];
        const auto [x, y] = sectionCoordinate(row, col);
        topology.section_grid_of_channel[fec][ch] = {x, y};
        topology.anode_grid_of_channel[fec][ch] = {origin_x + x, origin_y + y};
        global_to_fec_ch[origin_x + x][origin_y + y] = {fec, ch};
      }
    }

    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      const auto [x, y] = topology.section_grid_of_channel[fec][ch];

      for (const auto& delta : {std::pair<int, int>{-1, 0},
                                std::pair<int, int>{1, 0},
                                std::pair<int, int>{0, -1},
                                std::pair<int, int>{0, 1}}) {
        const int neighbor_x = x + delta.first;
        const int neighbor_y = y + delta.second;
        if (0 <= neighbor_x && neighbor_x < kFECSectionSidePixels &&
            0 <= neighbor_y && neighbor_y < kFECSectionSidePixels) {
          topology.section_cross_neighbors[fec][ch].push_back(
              channelAtSectionCoordinate(grid_to_ch, neighbor_x, neighbor_y));
        }
      }

      for (const auto& delta : {std::pair<int, int>{-1, -1},
                                std::pair<int, int>{-1, 1},
                                std::pair<int, int>{1, -1},
                                std::pair<int, int>{1, 1}}) {
        const int neighbor_x = x + delta.first;
        const int neighbor_y = y + delta.second;
        if (0 <= neighbor_x && neighbor_x < kFECSectionSidePixels &&
            0 <= neighbor_y && neighbor_y < kFECSectionSidePixels) {
          topology.section_diag_neighbors[fec][ch].push_back(
              channelAtSectionCoordinate(grid_to_ch, neighbor_x, neighbor_y));
        }
      }
    }

    std::vector<int> periphery;
    for (int x = 0; x < kFECSectionSidePixels; ++x) {
      for (int y = 0; y < kFECSectionSidePixels; ++y) {
        if (x == 0 || x == kFECSectionSidePixels - 1 ||
            y == 0 || y == kFECSectionSidePixels - 1) {
          periphery.push_back(channelAtSectionCoordinate(grid_to_ch, x, y));
        }
      }
    }

    std::sort(periphery.begin(), periphery.end());
    periphery.erase(std::unique(periphery.begin(), periphery.end()), periphery.end());
    topology.periphery[fec] = std::move(periphery);
  }

  for (int fec = 0; fec < NUM_VATA; ++fec) {
    for (int ch = 0; ch < NUM_CH_EACH_VATA; ++ch) {
      const auto [global_x, global_y] = topology.anode_grid_of_channel[fec][ch];

      for (const auto& delta : {std::pair<int, int>{-1, 0},
                                std::pair<int, int>{1, 0},
                                std::pair<int, int>{0, -1},
                                std::pair<int, int>{0, 1}}) {
        const int neighbor_x = global_x + delta.first;
        const int neighbor_y = global_y + delta.second;
        if (0 <= neighbor_x && neighbor_x < kTPCPlaneSidePixels &&
            0 <= neighbor_y && neighbor_y < kTPCPlaneSidePixels) {
          const auto [other_fec, other_ch] = global_to_fec_ch[neighbor_x][neighbor_y];
          if (other_fec >= 0 && other_fec != fec) {
            topology.cross_section_neighbors[fec][ch].push_back({other_fec, other_ch});
          }
        }
      }

      for (const auto& delta : {std::pair<int, int>{-1, -1},
                                std::pair<int, int>{-1, 1},
                                std::pair<int, int>{1, -1},
                                std::pair<int, int>{1, 1}}) {
        const int neighbor_x = global_x + delta.first;
        const int neighbor_y = global_y + delta.second;
        if (0 <= neighbor_x && neighbor_x < kTPCPlaneSidePixels &&
            0 <= neighbor_y && neighbor_y < kTPCPlaneSidePixels) {
          const auto [other_fec, other_ch] = global_to_fec_ch[neighbor_x][neighbor_y];
          if (other_fec >= 0 && other_fec != fec) {
            topology.diag_section_neighbors[fec][ch].push_back({other_fec, other_ch});
          }
        }
      }
    }
  }

  return topology;
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
