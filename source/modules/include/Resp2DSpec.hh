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

#ifndef COMPTONSOFT_Resp2DSpec_H
#define COMPTONSOFT_Resp2DSpec_H 1

#include "VCSModule.hh"

#include "globals.hh"

#include "EventReconstruction.hh"
#include "InitialInformation.hh"
#include "PhAbsModeSpectrum.hh"

#include "TH1.h"
#include "TH2.h"


namespace comptonsoft {

class Resp2DSpec : public VCSModule
{
  DEFINE_ANL_MODULE(Resp2DSpec, 1.1);
public:
  Resp2DSpec();
  ~Resp2DSpec() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  
private:
  EventReconstruction* EventReconstruction_ptr;
  PhAbsModeSpectrum* phabs_mode;
  const anlgeant4::InitialInformation* initial_info;

  TVector3 PlaneNormal;
  TVector3 PlanePoint;

  TH2F* hist_phabs;
  TH2F* hist_all;
  TH2F* hist_all_nf;
  std::vector<TH2F*> hist_dt02_vec;
  std::vector<TH2F*> hist_dt02_nf_vec;
  std::vector<TH2F*> hist_dt04_vec;
  std::vector<TH2F*> hist_dt04_nf_vec;
  std::vector<TH2F*> hist_dt06_vec;
  std::vector<TH2F*> hist_dt06_nf_vec;
  std::vector<TH2F*> hist_dt08_vec;
  std::vector<TH2F*> hist_dt08_nf_vec;
  std::vector<TH2F*> hist_dt12_vec;
  std::vector<TH2F*> hist_dt12_nf_vec;
  std::vector<TH2F*> hist_dt16_vec;
  std::vector<TH2F*> hist_dt16_nf_vec;
  std::vector<TH2F*> hist_dt24_vec;
  std::vector<TH2F*> hist_dt24_nf_vec;
  std::vector<TH2F*> hist_good_vec;
  std::vector<TH2F*> hist_good_nf_vec;
  std::vector<TH2F*> hist_good2_vec;
  std::vector<TH2F*> hist_good2_nf_vec;

  int NumBinEnergy;

  double RangeEnergy1;
  double RangeEnergy2;
};

}

#endif /* COMPTONSOFT_Resp2DSpec_H */


