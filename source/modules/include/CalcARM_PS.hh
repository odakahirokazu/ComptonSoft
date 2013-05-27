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

#ifndef COMPTONSOFT_CalcARM_PS_H
#define COMPTONSOFT_CalcARM_PS_H 1

#include "CalcARM.hh"

namespace comptonsoft {

class CalcARM_PS : public CalcARM
{
  DEFINE_ANL_MODULE(CalcARM_PS, 1.0);
public:
  CalcARM_PS();
  ~CalcARM_PS() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_ana();
  
private:
  TVector3 SourcePos;
};

}

#endif /* COMPTONSOFT_CalcARM_PS_H */


