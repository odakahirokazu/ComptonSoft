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

#ifndef COMPTONSOFT_FillFrame_H
#define COMPTONSOFT_FillFrame_H 1

#include "VCSModule.hh"
#include <vector>

namespace comptonsoft {

class CSHitCollection;
class ConstructFrame;

/**
 * @author Hirokazu Odaka
 * @date 2019-06-05
 * @date 2020-04-01 | v1.1
 */
class FillFrame : public VCSModule
{
  DEFINE_ANL_MODULE(FillFrame, 1.1);
public:
  FillFrame();
  ~FillFrame();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  CSHitCollection* m_HitCollection = nullptr;
  int m_offsetX = 0;
  int m_offsetY = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_FillFrame_H */
