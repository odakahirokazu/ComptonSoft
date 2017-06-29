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

#ifndef COMPTONSOFT_GenerateSimXEvent_H
#define COMPTONSOFT_GenerateSimXEvent_H 1

#include "BasicModule.hh"

namespace comptonsoft {

class SimXIF;
class CSHitCollection;


class GenerateSimXEvent : public anl::BasicModule
{
  DEFINE_ANL_MODULE(GenerateSimXEvent, 0.0);
public:
  GenerateSimXEvent();
  ~GenerateSimXEvent() {}
  
  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_ana() override;
  anl::ANLStatus mod_exit() override;

protected:
  CSHitCollection* m_HitCollection;

  int eventid;
  int seqnum;
  int totalhit;
  double ini_energy;

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
  
  double time1;

  unsigned int process;
  
  int grade;
  
  int detid;
  int stripx;
  int stripy;
  int chip;
  int channel;

  int time_group;

  unsigned int flag;

private:
  SimXIF* m_SimXIF;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_GenerateSimXEvent_H */
