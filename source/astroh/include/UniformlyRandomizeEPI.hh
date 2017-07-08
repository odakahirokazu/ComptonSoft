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

#ifndef COMPTONSOFT_UniformlyRandomizeEPI_H
#define COMPTONSOFT_UniformlyRandomizeEPI_H 1

#include "VCSModule.hh"

class TRandom;

namespace comptonsoft {

class CSHitCollection;

/**
 * uniformly randomize EPI for SGD data.
 * @author Hirokazu Odaka
 * @date 2017-05-01
 */
class UniformlyRandomizeEPI : public VCSModule
{
  DEFINE_ANL_MODULE(UniformlyRandomizeEPI, 1.0);
public:
  UniformlyRandomizeEPI();
  ~UniformlyRandomizeEPI();

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;
  
private:
  double m_LZDeltaE0 = 0.0;
  double m_LZDeltaE1 = 0.0;
  double m_HZDeltaE0 = 0.0;
  double m_HZDeltaE1 = 0.0;
  CSHitCollection* m_HitCollection = nullptr;
  std::unique_ptr<TRandom> m_Random;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_UniformlyRandomizeEPI_H */
