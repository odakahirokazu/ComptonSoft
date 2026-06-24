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

#include "MergeHitTree.hh"

#include "HitTreeIOWithInitialInfo.hh"

#include "TFile.h"
#include "TTree.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

using namespace anlnext;

namespace comptonsoft
{

namespace
{

constexpr const char* kHitTreeName = "hittree";

std::filesystem::path prepareOutputPath(const std::string& output_file)
{
  if (output_file.empty()) {
    throw std::runtime_error("MergeHitTree output is empty.");
  }

  std::filesystem::path output_path(output_file);
  const std::filesystem::path parent_path = output_path.parent_path();
  if (!parent_path.empty()) {
    std::filesystem::create_directories(parent_path);
  }
  return output_path;
}

std::unique_ptr<TFile> openInputFile(const std::string& filename)
{
  std::unique_ptr<TFile> input_file(TFile::Open(filename.c_str(), "READ"));
  if (!input_file || input_file->IsZombie()) {
    throw std::runtime_error("Failed to open input ROOT file: " + filename);
  }
  return input_file;
}

TTree* getHitTree(TFile& file, const std::string& filename)
{
  TTree* hittree = dynamic_cast<TTree*>(file.Get(kHitTreeName));
  if (!hittree) {
    throw std::runtime_error("Input ROOT file does not contain hittree: " + filename);
  }
  return hittree;
}

bool hasInitialInfoBranches(TTree* hittree)
{
  return hittree->GetBranch("ini_energy") != nullptr;
}

void configureInitialInfo(HitTreeIOWithInitialInfo& tree_io, bool has_initial_info)
{
  if (has_initial_info) {
    tree_io.enableInitialInfoRecord();
    return;
  }
  tree_io.disableInitialInfoRecord();
}

void copyInitialInfo(HitTreeIOWithInitialInfo& output_io,
                     const HitTreeIOWithInitialInfo& input_io)
{
  output_io.setInitialInfo(input_io.getInitialEnergy(),
                           input_io.getInitialDirection(),
                           input_io.getInitialTime(),
                           input_io.getInitialPosition(),
                           input_io.getInitialPolarization());
  output_io.setWeight(input_io.getWeight());
}

} /* anonymous namespace */

ANLStatus MergeHitTree::mod_define()
{
  define_parameter("hittree_files", &mod_class::hittree_files_);
  define_parameter("output",        &mod_class::output_);
  define_parameter("overwrite",     &mod_class::overwrite_);
  return AS_OK;
}

ANLStatus MergeHitTree::mod_initialize()
{
  VCSModule::mod_initialize();
  mergeHitTrees();
  return AS_OK;
}

ANLStatus MergeHitTree::mod_analyze()
{
  return AS_QUIT;
}

void MergeHitTree::mergeHitTrees() const
{
  if (hittree_files_.empty()) {
    throw std::runtime_error("MergeHitTree hittree_files is empty.");
  }

  const std::filesystem::path output_path = prepareOutputPath(output_);
  const char* output_mode = "CREATE";
  if (overwrite_) {
    output_mode = "RECREATE";
  }

  std::unique_ptr<TFile> output_file(new TFile(output_path.string().c_str(), output_mode));
  if (!output_file || output_file->IsZombie()) {
    throw std::runtime_error("Failed to create output ROOT file: " +
                             output_path.string());
  }

  std::unique_ptr<TTree> output_tree(new TTree(kHitTreeName, kHitTreeName));
  output_tree->SetDirectory(nullptr);

  bool initial_info_layout_is_known = false;
  bool output_has_initial_info = false;
  HitTreeIOWithInitialInfo output_io;
  output_io.setTree(output_tree.get());

  int64_t output_event_id = 0;
  int64_t total_entries = 0;
  int64_t total_events = 0;

  for (const std::string& filename : hittree_files_) {
    std::unique_ptr<TFile> input_file = openInputFile(filename);
    TTree* input_tree = getHitTree(*input_file, filename);
    const bool input_has_initial_info = hasInitialInfoBranches(input_tree);

    if (!initial_info_layout_is_known) {
      output_has_initial_info = input_has_initial_info;
      configureInitialInfo(output_io, output_has_initial_info);
      output_io.defineBranches();
      initial_info_layout_is_known = true;
    }
    else if (input_has_initial_info != output_has_initial_info) {
      throw std::runtime_error(
          "Cannot merge hittrees with different initial-information branch layouts.");
    }

    HitTreeIOWithInitialInfo input_io;
    input_io.setTree(input_tree);
    configureInitialInfo(input_io, input_has_initial_info);
    input_io.setBranchAddresses();

    int64_t entry = 0;
    const int64_t num_entries = input_tree->GetEntries();
    while (entry < num_entries) {
      input_tree->GetEntry(entry);
      if (output_has_initial_info) {
        copyInitialInfo(output_io, input_io);
      }

      const int64_t previous_entry = entry;
      const std::vector<DetectorHit_sptr> hits =
          input_io.retrieveHits(entry, false);
      if (entry <= previous_entry) {
        throw std::runtime_error("Invalid hittree entry grouping in: " + filename);
      }

      output_io.fillHits(output_event_id, hits);
      ++output_event_id;
      ++total_events;
    }
    total_entries += num_entries;
  }

  output_file->cd();
  output_tree->Write();
  output_file->Write();
  const int64_t output_entries = output_tree->GetEntries();
  output_file->Close();

  std::cout << "[ROOT] Merged " << hittree_files_.size()
            << " hittree files into " << output_path << std::endl;
  std::cout << "[ROOT] Input entries: " << total_entries
            << ", output events: " << total_events
            << ", output entries: " << output_entries << std::endl;
}

} /* namespace comptonsoft */
