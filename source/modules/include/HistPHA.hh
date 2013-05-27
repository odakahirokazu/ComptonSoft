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

#ifndef COMPTONSOFT_HistPHA_H
#define COMPTONSOFT_HistPHA_H 1

#include "VCSModule.hh"
#include "TH1.h"

namespace comptonsoft {

class HistPHA : public VCSModule
{
  DEFINE_ANL_MODULE(HistPHA, 2.1);
public:
  HistPHA();
  ~HistPHA() {}
 
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

private:
  std::vector<TH1*> m_hisSpec;
  int m_NBin;
  double m_PHAMin;
  double m_PHAMax;
};

}

#endif /* COMPTONSOFT_HistPHA_H */
