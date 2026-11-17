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

#include <fitsio.h>
#include <TFile.h>
#include <TLeaf.h>
#include <TTree.h>
#include <array>
#include <cmath>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  if (argc != 3) { return 1; }
  TFile input(argv[1]);
  auto* tree = input.Get<TTree>("eventtree");
  if (!tree) { return 1; }
  int files = 0;
  for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
    tree->GetEntry(i);
    const int n = tree->GetLeaf("num_hits")->GetValue();
    if (!n) { continue; }
    const int event = tree->GetLeaf("eventid")->GetValue();
    std::array<double, 256> expected_energy{}, energy{};
    std::array<long, 256> expected_counts{}, counts{};
    for (int h = 0; h < n; ++h) {
      const int x = tree->GetLeaf("pixelx")->GetValue(h);
      const int y = tree->GetLeaf("pixely")->GetValue(h);
      if (x < 0 || x >= 16 || y < 0 || y >= 16) { return 1; }
      expected_energy[y*16+x] += tree->GetLeaf("epi")->GetValue(h);
      ++expected_counts[y*16+x];
    }
    const std::string name = std::string(argv[2]) + "_Frame" + std::to_string(event) + ".fits";
    fitsfile* file = nullptr;
    int status = 0, dimensions = 0, any_null = 0, hdu_type = 0;
    long axes[2] = {0, 0};
    fits_open_file(&file, name.c_str(), READONLY, &status);
    if (status) { fits_report_error(stderr, status); return 1; }
    fits_get_img_dim(file, &dimensions, &status);
    fits_get_img_size(file, 2, axes, &status);
    fits_read_img(file, TDOUBLE, 1, 256, nullptr, energy.data(), &any_null, &status);
    fits_movabs_hdu(file, 2, &hdu_type, &status);
    fits_read_img(file, TLONG, 1, 256, nullptr, counts.data(), &any_null, &status);
    fits_close_file(file, &status);
    if (status || dimensions != 2 || axes[0] != 16 || axes[1] != 16) { return 1; }
    for (int p = 0; p < 256; ++p) {
      if (std::abs(energy[p] - expected_energy[p]) > 1.0e-6 || counts[p] != expected_counts[p]) {
        std::cerr << "FITS differs from ROOT input: " << name << '\n';
        return 1;
      }
    }
    ++files;
  }
  std::cout << files << " FITS frames: dimensions, energy and counts match the input\n";
  return files > 0 ? 0 : 1;
}
