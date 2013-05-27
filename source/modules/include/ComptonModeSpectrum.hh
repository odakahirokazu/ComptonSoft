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

#ifndef COMPTONSOFT_ComptonModeSpectrum_H
#define COMPTONSOFT_ComptonModeSpectrum_H 1

#include "VCSModule.hh"

#include "globals.hh"

#include "EventReconstruction.hh"

#include "TH1.h"

namespace comptonsoft {

class ComptonModeSpectrum : public VCSModule
{
  DEFINE_ANL_MODULE(ComptonModeSpectrum, 1.2);
public:
  ComptonModeSpectrum();
  ~ComptonModeSpectrum() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
    
private:
  EventReconstruction* EventReconstruction_ptr;

  TVector3 PlaneNormal;
  TVector3 PlanePoint;

  TH1D* hist_all;
  TH1D* hist_all_nf;
  std::vector<TH1D*> hist_vec;
  std::vector<TH1D*> hist_nf_vec;

  TH1D* hist_all_dt02;
  TH1D* hist_all_nf_dt02;
  std::vector<TH1D*> hist_dt02_vec;
  std::vector<TH1D*> hist_nf_dt02_vec;

  TH1D* hist_all_dt04;
  TH1D* hist_all_nf_dt04;
  std::vector<TH1D*> hist_dt04_vec;
  std::vector<TH1D*> hist_nf_dt04_vec;

  TH1D* hist_all_dt06;
  TH1D* hist_all_nf_dt06;
  std::vector<TH1D*> hist_dt06_vec;
  std::vector<TH1D*> hist_nf_dt06_vec;

  TH1D* hist_all_dt08;
  TH1D* hist_all_nf_dt08;
  std::vector<TH1D*> hist_dt08_vec;
  std::vector<TH1D*> hist_nf_dt08_vec;

  TH1D* hist_all_dt12;
  TH1D* hist_all_nf_dt12;
  std::vector<TH1D*> hist_dt12_vec;
  std::vector<TH1D*> hist_nf_dt12_vec;

  TH1D* hist_all_dt16;
  TH1D* hist_all_nf_dt16;
  std::vector<TH1D*> hist_dt16_vec;
  std::vector<TH1D*> hist_nf_dt16_vec;

  TH1D* hist_all_dt24;
  TH1D* hist_all_nf_dt24;
  std::vector<TH1D*> hist_dt24_vec;
  std::vector<TH1D*> hist_nf_dt24_vec;

  TH1D* hist_all_good;
  TH1D* hist_all_nf_good;
  std::vector<TH1D*> hist_good_vec;
  std::vector<TH1D*> hist_nf_good_vec;

  TH1D* hist_all_good2;
  TH1D* hist_all_nf_good2;
  std::vector<TH1D*> hist_good2_vec;
  std::vector<TH1D*> hist_nf_good2_vec;

  int NumBinEnergy;

  double RangeEnergy1;
  double RangeEnergy2;
};

}

#endif /* COMPTONSOFT_ComptonModeSpectrum_H */
