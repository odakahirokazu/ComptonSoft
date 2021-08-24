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

#ifndef COMPTONSOFT_LoadReducedFrame_H
#define COMPTONSOFT_LoadReducedFrame_H 1

#include <anlnext/BasicModule.hh>
#include <unordered_set>
#include "VDataReader.hh"

namespace comptonsoft {

class FrameData;

class LoadReducedFrame : public anlnext::BasicModule, public VDataReader
{
  DEFINE_ANL_MODULE(LoadReducedFrame, 1.0);

public:
  LoadReducedFrame();
  
protected:
  LoadReducedFrame(const LoadReducedFrame&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void addFile(const std::string& filename) override;
  bool hasFile(const std::string& filename) const override;
  bool isDone() const override;

private:
  int detector_id_ = 0;
  std::vector<std::string> files_;
  std::unordered_set<std::string> past_files_;
  FrameData* frame_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_LoadReducedFrame_H */
