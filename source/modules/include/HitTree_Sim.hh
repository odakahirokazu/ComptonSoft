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

// 2008-08-28 Hirokazu Odaka
// 2011-04-27 Hirokazu Odaka

#ifndef COMPTONSOFT_HitTree_Sim_H
#define COMPTONSOFT_HitTree_Sim_H 1

#include "HitTree.hh"

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {

class HitTree_Sim : public HitTree
{
  DEFINE_ANL_MODULE(HitTree_Sim, 1.2);
public:
  HitTree_Sim();
  ~HitTree_Sim() {}

  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

protected:
  double ini_dirx;
  double ini_diry;
  double ini_dirz;
  double ini_time;
  double ini_posx;
  double ini_posy;
  double ini_posz;
  double ini_polarx;
  double ini_polary;
  double ini_polarz;
  double weight;

private:
  const anlgeant4::InitialInformation* initial_info;
  bool polarization_enable;
};

}

#endif /* COMPTONSOFT_HitTree_Sim_H */
