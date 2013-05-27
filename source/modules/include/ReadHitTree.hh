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

#ifndef COMPTONSOFT_ReadHitTree_H
#define COMPTONSOFT_ReadHitTree_H 1

#include <vector>
#include <iostream>

#include "G4ThreeVector.hh"

#include "BasicModule.hh"
#include "InitialInformation.hh"

#include "TChain.h"
#include "CSHitCollection.hh"


namespace comptonsoft {

class ReadHitTree : public anl::BasicModule, public anlgeant4::InitialInformation
{
  DEFINE_ANL_MODULE(ReadHitTree, 1.4);
public:
  ReadHitTree();
  ~ReadHitTree() {}
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  std::vector <std::string> inputfilename;

  TChain* hittree;
  Long64_t nevent;
  Long64_t id;

  CSHitCollection* hit_collection;

  int eventid;
  int seqnum;
  int totalhit;
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

  double realposx;
  double realposy;
  double realposz;

  double localposx;
  double localposy;
  double localposz;

  double posx;
  double posy;
  double posz;

  double edep;
  double e_pha;
  double e_pi;
  
  double time;

  unsigned int process;
  
  int grade;
  
  int detid;
  int stripx;
  int stripy;
  int chip;
  int channel;

  int time_group;

  unsigned int flag;
};

}

#endif /* COMPTONSOFT_ReadHitTree_H */
