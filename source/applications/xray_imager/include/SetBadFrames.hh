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

/**
 * SetBadFrames.
 *
 * @author Tsubasa Tamba
 * @date 2019-07-24
 * @date 2020-04-01 | Hirokazu Odaka | use module-parameter | v1.1
 *
 */

#ifndef COMPTONSOFT_SetBadFrames_H
#define COMPTONSOFT_SetBadFrames_H 1

#include "VCSModule.hh"
#include "FrameData.hh"

namespace comptonsoft {

class SetBadFrames : public VCSModule
{
  DEFINE_ANL_MODULE(SetBadFrames, 1.1);
  // ENABLE_PARALLEL_RUN();
public:
  SetBadFrames();
  
public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  int detectorID_ = 0;
  std::vector<int> badFrames_;
  std::vector<int>::iterator nextBadFrame_;
  FrameData* frame_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetBadFrames_H */
