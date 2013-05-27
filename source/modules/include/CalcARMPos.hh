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

#ifndef COMPTONSOFT_CalcARMPos_H
#define COMPTONSOFT_CalcARMPos_H 1

#include "CalcARM.hh"

namespace comptonsoft {

class CalcARMPos : public CalcARM
{
  DEFINE_ANL_MODULE(CalcARMPos, 2.0);
public:
  CalcARMPos();
  ~CalcARMPos() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_ana();
  
private:
  vector3_t SourceDirection;
};

}

#endif /* COMPTONSOFT_CalcARMPos_H */
