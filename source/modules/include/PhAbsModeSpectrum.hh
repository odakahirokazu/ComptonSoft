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

#ifndef COMPTONSOFT_PhAbsModeSpectrum_H
#define COMPTONSOFT_PhAbsModeSpectrum_H 1

#include "VCSModule.hh"

#include "globals.hh"

#include "EventReconstruction.hh"

#include "TH1.h"

namespace comptonsoft {

class PhAbsModeSpectrum : public VCSModule
{
  DEFINE_ANL_MODULE(PhAbsModeSpectrum, 1.0);
public:
  PhAbsModeSpectrum();
  ~PhAbsModeSpectrum() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

  double Energy() { return energy; }
  
private:
  CSHitCollection* hit_collection;

  TH1D* hist_phall;
  TH1D* hist_1hit;
  TH1D* hist_2hit;
  TH1D* hist_3hit;
  TH1D* hist_4hit;
  TH1D* hist_mhit;

  int NumBinEnergy;

  double RangeEnergy1;
  double RangeEnergy2;

  double energy;
};

}

#endif /* COMPTONSOFT_PhAbsModeSpectrum_H */

