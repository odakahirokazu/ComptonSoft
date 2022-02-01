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
#include "TTree.h"
#include "TFile.h"
#include "FrameData.hh"

using namespace anlnext;

namespace comptonsoft {

LoadReducedFrame::LoadReducedFrame()
{
}

bool LoadReducedFrame::load(FrameData* frame, const std::string& filename)
{
  frame->resetRawFrame();
  frame->clearEventCheckPixels();
  image_t& rawFrame = frame->getRawFrame();

  TFile f(filename.c_str());
  TTree* tree = static_cast<TTree*>(f.Get("rawtree"));
  int ix = 0;
  int iy = 0;
  double ph = 0;
  tree->SetBranchAddress("ph", &ph);
  tree->SetBranchAddress("x", &ix);
  tree->SetBranchAddress("y", &iy);
  const int num_entries = tree->GetEntries();
  for (int i=0; i<num_entries; i++) {
    tree->GetEntry(i);
    rawFrame[ix][iy] = ph;
    frame->addEventCheckPixels(ix, iy);
  }
  f.Close();

  return true;
}

} /* namespace comptonsoft */
