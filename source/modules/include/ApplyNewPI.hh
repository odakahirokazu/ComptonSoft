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

// 2011-06-23 Hirokazu Odaka

#ifndef COMPTONSOFT_ApplyNewPI_H
#define COMPTONSOFT_ApplyNewPI_H 1

#include "MakePI.hh"

namespace comptonsoft {

class ApplyNewPI : public MakePI
{
  DEFINE_ANL_MODULE(ApplyNewPI, 1.0);
public:
  ApplyNewPI() {}
  ~ApplyNewPI() {}

  anl::ANLStatus mod_ana();
};

}

#endif /* COMPTONSOFT_ApplyNewPI_H */
