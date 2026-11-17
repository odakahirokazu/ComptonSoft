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

#include <TFile.h>
#include <TLeaf.h>
#include <TTree.h>
#include <TSystem.h>
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

// Run with ROOT: check_trees.C("/path/to/results"). Nonzero exit on failure.
void check_trees(const char* directory)
{
  try {
    const std::string base = std::string(directory) + "/";
    for (const auto* name: {"pixel", "stacked", "strip", "pixel_mt"}) {
      TFile file((base + name + ".root").c_str());
      auto* tree = file.Get<TTree>("hittree");
      if (file.IsZombie() || !tree || tree->GetEntries() == 0) {
        throw std::runtime_error(std::string("missing hits: ") + name);
      }
      std::set<std::pair<int, int>> events;
      for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        const int run = tree->GetLeaf("runid")->GetValue();
        const int event = tree->GetLeaf("eventid")->GetValue();
        if (run < 0 || run >= 5 || event < 0 || event >= 200 ||
            !std::isfinite(tree->GetLeaf("epi")->GetValue())) {
          throw std::runtime_error(std::string("invalid hit: ") + name);
        }
        events.emplace(run, event);
      }
      std::cout << name << ": " << tree->GetEntries() << " hits, "
                << events.size() << " detected events\n";
    }

    for (const auto* name: {"eventtree", "raw"}) {
      TFile file((base + name + ".root").c_str());
      auto* tree = file.Get<TTree>("eventtree");
      if (file.IsZombie() || !tree || tree->GetEntries() != 1001) {
        throw std::runtime_error(std::string("incorrect event count: ") + name);
      }
      int empty = 0;
      Long64_t hits = 0;
      for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        if (tree->GetLeaf("runid")->GetValue() != i / 200 ||
            tree->GetLeaf("eventid")->GetValue() != i % 200) {
          throw std::runtime_error("event lost, duplicated or reordered");
        }
        const int n = tree->GetLeaf("num_hits")->GetValue();
        empty += n == 0;
        hits += n;
        if (std::string(name) == "raw") {
          for (int h = 0; h < n; ++h) {
            for (const auto* axis: {"x", "y", "z"}) {
              const auto local = tree->GetLeaf((std::string("local_pos") + axis).c_str())->GetValue(h);
              const auto world = tree->GetLeaf((std::string("real_pos") + axis).c_str())->GetValue(h);
              if (!std::isfinite(local) || std::abs(local - world) > 1.0e-6) {
                throw std::runtime_error("raw local position was changed");
              }
            }
          }
        }
      }
      if (!empty || !hits) { throw std::runtime_error("fixture needs empty and detected events"); }
      std::cout << name << ": 1001 events, " << hits << " hits, " << empty << " empty\n";
    }

    TFile input((base + "no_initial_info.root").c_str());
    TFile output((base + "rewritten.root").c_str());
    auto* original = input.Get<TTree>("eventtree");
    auto* events = output.Get<TTree>("eventtree");
    auto* hits = output.Get<TTree>("hittree");
    if (!original || !events || !hits || original->GetEntries() != events->GetEntries()) {
      throw std::runtime_error("roundtrip event count changed");
    }
    for (auto* tree: {events, hits}) {
      for (auto* branch: *tree->GetListOfBranches()) {
        const std::string name = branch->GetName();
        if (name.rfind("ini_", 0) == 0 || name == "weight") {
          throw std::runtime_error("invented initial information");
        }
      }
    }
    Long64_t expected_hits = 0;
    for (Long64_t i = 0; i < original->GetEntries(); ++i) {
      original->GetEntry(i);
      events->GetEntry(i);
      for (const auto* field: {"runid", "eventid", "num_hits"}) {
        if (original->GetLeaf(field)->GetValue() != events->GetLeaf(field)->GetValue()) {
          throw std::runtime_error(std::string("roundtrip changed ") + field);
        }
      }
      expected_hits += original->GetLeaf("num_hits")->GetValue();
    }
    if (hits->GetEntries() != expected_hits) { throw std::runtime_error("roundtrip hit count changed"); }
    std::cout << "No-initial-info roundtrip: " << events->GetEntries() << " events, "
              << hits->GetEntries() << " hits; no fabricated initial branches\n";
  }
  catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    gSystem->Exit(1);
  }
}
