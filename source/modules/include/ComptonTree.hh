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

#ifndef COMPTONSOFT_ComptonTree_H
#define COMPTONSOFT_ComptonTree_H 1

#include "VCSModule.hh"
#include "globals.hh"

class TTree;

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {

class EventReconstruction;


class ComptonTree : public VCSModule
{
  DEFINE_ANL_MODULE(ComptonTree, 1.2);
public:
  ComptonTree();
  ~ComptonTree() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  EventReconstruction* EventReconstruction_ptr;
  const anlgeant4::InitialInformation* initial_info;
  TTree * comptree;
  bool detector_detail;
  bool polarization_enable;

  int eventid;
  double h1time;
  int h1process;
  int h1detid;
  int h1stripx;
  int h1stripy;
  int h1chip;
  int h1channel;
  double h1edep;
  double h1posx;
  double h1posy;
  double h1posz;
  double h2time;
  int h2process;
  int h2detid;
  int h2stripx;
  int h2stripy;
  int h2chip;
  int h2channel;
  double h2edep;
  double h2posx;
  double h2posy;
  double h2posz;
  double costheta_kin;
  double dtheta;
  unsigned int hitpattern;
  unsigned int flag;
  double ini_energy;
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
};

}

#endif /* COMPTONSOFT_ComptonTree_H */


