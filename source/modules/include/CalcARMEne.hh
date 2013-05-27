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

#ifndef COMPTONSOFT_CalcARMEne_H
#define COMPTONSOFT_CalcARMEne_H 1

#include <boost/tuple/tuple.hpp>

#include "CalcARM.hh"

namespace comptonsoft {

class CalcARMEne : public CalcARM
{
  DEFINE_ANL_MODULE(CalcARMEne, 2.0);
public:
  CalcARMEne() {}
  ~CalcARMEne() {}
  
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_ana();
  
private:
  double calculateNoiseFWHM(const std::string& name, double edep);
  std::map<std::string, boost::tuple<double, double, double> > noiselevel_map;
};

}

#endif /* COMPTONSOFT_CalcARMEne_H */
