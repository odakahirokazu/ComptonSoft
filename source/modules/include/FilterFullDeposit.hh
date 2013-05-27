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

#ifndef COMPTONSOFT_FilterFullDeposit_H
#define COMPTONSOFT_FilterFullDeposit_H 1

#include "BasicModule.hh"
#include "globals.hh"

#include "CSHitCollection.hh"

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {

class FilterFullDeposit : public anl::BasicModule
{
  DEFINE_ANL_MODULE(FilterFullDeposit, 1.0);
public:
  FilterFullDeposit() {}
  ~FilterFullDeposit() {}

  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  
private:
  CSHitCollection* hit_collection;
  const anlgeant4::InitialInformation* initial_info;
};

}

#endif /* COMPTONSOFT_FilterFullDeposit_H */


