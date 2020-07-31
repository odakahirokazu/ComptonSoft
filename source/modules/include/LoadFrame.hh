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

#ifndef COMPTONSOFT_LoadFrame_H
#define COMPTONSOFT_LoadFrame_H 1

#include <anlnext/BasicModule.hh>
#include "VDataReader.hh"

namespace comptonsoft {

class FrameData;


/**
 * LoadFrame
 *
 * @author Hirokazu Odaka
 * @date 2019-05-23
 * @date 2020-04-01 | upgrade for new ConstructFrame
 */
class LoadFrame : public anlnext::BasicModule, public VDataReader
{
  DEFINE_ANL_MODULE(LoadFrame, 1.1);
  // ENABLE_PARALLEL_RUN();
public:
  LoadFrame();
  
protected:
  LoadFrame(const LoadFrame&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void addFile(const std::string& filename) override;
  bool hasFile(const std::string& filename) const override;
  bool isDone() const override;

private:
  bool byte_order_ = true;
  int detector_id_ = 0;
  std::vector<std::string> files_;
  FrameData* frame_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_LoadFrame_H */
