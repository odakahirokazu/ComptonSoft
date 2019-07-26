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

#include "SetBadFrames.hh"
#include "FrameData.hh"
#include <fstream>

using namespace anlnext;

namespace comptonsoft{

SetBadFrames::SetBadFrames()
  : filename_("hotpix.txt")
{
}

ANLStatus SetBadFrames::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}

ANLStatus SetBadFrames::mod_initialize()
{
  get_module_NC("ConstructFrame", &frame_owner_);
  return AS_OK;
}

ANLStatus SetBadFrames::mod_begin_run()
{
  std::ifstream fin(filename_);
  int x=0;
  while (fin >> x) {
    badFrames_.push_back(x);
  }
  fin.close();

  return AS_OK;
}

ANLStatus SetBadFrames::mod_analyze()
{
  FrameData& frameData = frame_owner_->getFrame();
  const int frameID = frame_owner_->FrameID();
  if (frameID==badFrames_.front()) {
    frameData.setBadFrame(true);
    badFrames_.pop_front();
  } 
  else {
    frameData.setBadFrame(false);
  }

  return AS_OK;
}

} /* namespace comptonsoft */
