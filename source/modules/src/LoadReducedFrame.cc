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

#include "LoadReducedFrame.hh"
#include <algorithm>
#include "FrameData.hh"
#include "ConstructDetector.hh"
#include "DetectorSystem.hh"
#include "VRealDetectorUnit.hh"

#include "TTree.h"
#include "TFile.h"

using namespace anlnext;

namespace comptonsoft {

LoadReducedFrame::LoadReducedFrame()
{
}

ANLStatus LoadReducedFrame::mod_define()
{
  define_parameter("files", &mod_class::files_);
  
  return AS_OK;
}

ANLStatus LoadReducedFrame::mod_initialize()
{
  ConstructDetector* detectorOwner;
  get_module_NC("ConstructDetector", &detectorOwner);
  VRealDetectorUnit* detector = detectorOwner->getDetectorManager()->getDetectorByID(detector_id_);
  if (detector == nullptr) {
    std::cout << "Detector " << detector_id_ << " does not exist." << std::endl;
    return AS_QUIT;
  }

  if (detector->hasFrameData()) {
    frame_ = detector->getFrameData();
  } else { 
    std::cout << "Detector does not have a frame." << std::endl;
    return AS_QUIT;
  }

  return AS_OK;
}

ANLStatus LoadReducedFrame::mod_analyze()
{
  const std::size_t fileIndex = get_loop_index();
  if (fileIndex >= files_.size()) {
    return AS_QUIT;
  }
  const std::string filename = files_[fileIndex];
  std::cout << "[LoadReducedFrame] filename: " << filename << std::endl;

  frame_->setFrameID(fileIndex);
  frame_->resetRawFrame();
  image_t& rawFrame = frame_->getRawFrame();

  TFile* f = new TFile(filename.c_str());
  TTree* tree_ = (TTree*)f->Get("rawtree");
  int ix = 0;
  int iy = 0;
  double ph = 0;
  tree_->SetBranchAddress("ph", &ph);
  tree_->SetBranchAddress("x", &ix);
  tree_->SetBranchAddress("y", &iy);
  const int num_entries = tree_->GetEntries();

  for (int i=0; i<num_entries; i++) {
    tree_->GetEntry(i);
    rawFrame[ix][iy] = ph;
  }
  f->Close();

  return AS_OK;
}

void LoadReducedFrame::addFile(const std::string& filename)
{
  files_.push_back(filename);
  past_files_.insert(filename);
}

bool LoadReducedFrame::hasFile(const std::string& filename) const
{
  return past_files_.count(filename) != 0;
}

bool LoadReducedFrame::isDone() const
{
  const std::size_t fileIndex = get_loop_index();
  return (fileIndex >= files_.size());
}

} /* namespace comptonsoft */
