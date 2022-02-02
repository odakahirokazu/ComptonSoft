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

#include "LoadFrame.hh"

namespace comptonsoft {

/**
 * LoadReducedFrame
 * 
 * @author Taihei Watanabe
 * @date 2021-07-29
 * @date 2022-02-01 | 1.2 | Hirokazu Odaka | derived from LoadFrame
 */
class LoadReducedFrame : public LoadFrame
{
  DEFINE_ANL_MODULE(LoadReducedFrame, 1.2);

public:
  LoadReducedFrame();

protected:
  bool load(FrameData* frame, const std::string& filename) override;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_LoadReducedFrame_H */
