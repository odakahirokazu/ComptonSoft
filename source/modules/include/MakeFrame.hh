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

#ifndef COMPTONSOFT_MakeFrame_H
#define COMPTONSOFT_MakeFrame_H 1

#include "VCSModule.hh"
#include <vector>

namespace comptonsoft {

class CSHitCollection;

/**
 * @author Tsubasa Tamba, Hirokazu Odaka
 * @date 2019-04-16 | created by T. Tamba
 * @date 2019-04-19 | cleanup by H. Odaka
 */
class MakeFrame : public VCSModule
{
  DEFINE_ANL_MODULE(MakeFrame, 1.0);
public:
  MakeFrame();
  ~MakeFrame();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  std::string m_FileNameBase;
  int m_NumPixelX = 1;
  int m_NumPixelY = 1;

  CSHitCollection* m_HitCollection = nullptr;
  long m_NumPixelsArray[2] = {1, 1};
  long m_NumPixels = 1;
  std::vector<double> m_EnergyArray;
  std::vector<long> m_CountArray;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_MakeFrame_H */
