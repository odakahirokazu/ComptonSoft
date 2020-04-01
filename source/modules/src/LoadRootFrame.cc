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

#include "LoadRootFrame.hh"
#include "FrameData.hh"
#include "ConstructFrame.hh"

using namespace anlnext;

namespace comptonsoft {

LoadRootFrame::LoadRootFrame()
  : treename_("XRPIX_Data"),
    branchname_("ADC")
{
}

ANLStatus LoadRootFrame::mod_define()
{
  define_parameter("files", &mod_class::files_);
  define_parameter("tree_name", &mod_class::treename_);
  define_parameter("branch_name", &mod_class::branchname_);
  
  return AS_OK;
}

ANLStatus LoadRootFrame::mod_initialize()
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
  }
  else {
    std::cout << "Detector does not have a frame." << std::endl;
    return AS_QUIT;
  }

  const int nx = frame_->NumPixelsX();
  const int ny = frame_->NumPixelsX();
  rawPH_.resize(boost::extents[nx][ny]);
  rawFrame_.resize(boost::extents[nx][ny]);

  frametree_ = new TChain(treename_.c_str());
  for (const std::string& filename: files_) {
    frametree_->Add(filename.c_str());
  }
  frametree_->SetBranchAddress(branchname_.c_str(), &rawPH_[0][0]);
  numEntries_ = frametree_->GetEntries();

  return AS_OK;
}

ANLStatus LoadRootFrame::mod_analyze()
{
  const std::size_t frameIndex = get_loop_index();
  if (frameIndex == numEntries_) {
    return AS_QUIT;
  }

  frame_->setFrameID(frameIndex);
  frametree_->GetEntry(frameIndex);

  const int nx = frame_->NumPixelsX();
  const int ny = frame_->NumPixelsX();
  for (int i=0; i<nx; i++) {
    for (int j=0; j<ny; j++) {
      rawFrame_[i][j] = static_cast<double>(rawPH_[i][j]);
    }
  }

  frame_->setRawFrame(rawFrame_);

  return AS_OK;
}

} /* namespace comptonsoft */
