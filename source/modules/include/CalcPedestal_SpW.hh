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

#ifndef COMPTONSOFT_CalcPedestal_SpW_H
#define COMPTONSOFT_CalcPedestal_SpW_H 1

#include "VCSModule.hh"
#include "TH1.h"

namespace comptonsoft {

class CalcPedestal_SpW : public VCSModule
{
  DEFINE_ANL_MODULE(CalcPedestal_SpW, 2.0);
public:
  CalcPedestal_SpW() {}
  ~CalcPedestal_SpW() {}

  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_endrun();
  
private:
  std::vector<TH1*> m_hisSpec;
  std::vector<TH1*> m_hisPed;
  std::vector<TH1*> m_hisNoise;
};

}

#endif /* COMPTONSOFT_CalcPedestal_SpW_H */
