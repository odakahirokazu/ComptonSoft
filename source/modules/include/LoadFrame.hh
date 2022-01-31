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
#include <unordered_set>
#include "VDataReader.hh"

namespace comptonsoft {

class FrameData;


/**
 * LoadFrame
 *
 * @author Hirokazu Odaka
 * @date 2019-05-23
 * @date 2020-04-01 | upgrade for new ConstructFrame
 * @date 2021-09-30 | Taihei Watanabe | use a hash for checking file overlap
 */
class LoadFrame : public anlnext::BasicModule, public VDataReader
{
  DEFINE_ANL_MODULE(LoadFrame, 1.2);
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

  std::string Filename() override { return filename_; };

private:
  bool byte_order_ = true;
  int odd_row_pixel_shift_ = 0;
  int start_position_ = 0;
  bool read_direction_x_ = false;
  int detector_id_ = 0;
  std::vector<std::string> files_;
  std::unordered_set<std::string> file_hash_;
  FrameData* frame_ = nullptr;
  std::string filename_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_LoadFrame_H */
