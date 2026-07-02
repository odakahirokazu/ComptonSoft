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

#ifndef COMPTONSOFT_ConstructSXIFrame_H
#define COMPTONSOFT_ConstructSXIFrame_H 1

#include "ConstructFrame.hh"

namespace comptonsoft {

/**
 * ConstructFrame
 *
 * @author Hirokazu Odaka
 * @date 2019-06-05
 * @date 2020-04-01 | v2.0 | upgrade with ConstructFrame 2.0
 */
class ConstructSXIFrame : public ConstructFrame
{
  DEFINE_ANL_MODULE(ConstructSXIFrame, 2.0);
  // ENABLE_PARALLEL_RUN();
public:
  ConstructSXIFrame() = default;

protected:
  std::unique_ptr<FrameData> createFrameData() override;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ConstructSXIFrame_H */
