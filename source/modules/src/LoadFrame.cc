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

#include "LoadFrame.hh"
#include <algorithm>
#include "FrameData.hh"
#include "ConstructFrame.hh"

using namespace anlnext;

namespace comptonsoft {

LoadFrame::LoadFrame()
{
}

ANLStatus LoadFrame::mod_define()
{
  define_parameter("files", &mod_class::files_);
  
  return AS_OK;
}

ANLStatus LoadFrame::mod_initialize()
{
  get_module_NC("ConstructFrame", &frame_owner_);

  return AS_OK;
}

ANLStatus LoadFrame::mod_analyze()
{
  if (isDone()) {
    return AS_QUIT;
  }

  const std::size_t fileIndex = get_loop_index();
  const std::string filename = files_[fileIndex];
  std::cout << "[LoadFrame] filename: " << filename << std::endl;

  frame_owner_->setFrameID(fileIndex);
  FrameData& frame = frame_owner_->getFrame();
  bool status = frame.load(filename);
  if (!status) {
    return AS_ERROR;
  }

  return AS_OK;
}

void LoadFrame::addFile(const std::string& filename)
{
  files_.push_back(filename);
}

bool LoadFrame::hasFile(const std::string& filename) const
{
  return (std::find(files_.begin(), files_.end(), filename) != files_.end());
}

bool LoadFrame::isDone() const
{
  const std::size_t fileIndex = get_loop_index();
  return (fileIndex == files_.size());
}

} /* namespace comptonsoft */
