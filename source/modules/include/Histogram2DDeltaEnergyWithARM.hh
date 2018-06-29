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

#ifndef COMPTONSOFT_Histogram2DDeltaEnergyWithARM_H
#define COMPTONSOFT_Histogram2DDeltaEnergyWithARM_H 1

#include "VCSModule.hh"

class TH2;

namespace anlgeant4 {
class InitialInformation;
}

namespace comptonsoft {

class EventReconstruction;

class Histogram2DDeltaEnergyWithARM : public VCSModule
{
  DEFINE_ANL_MODULE(Histogram2DDeltaEnergyWithARM, 3.1)
public:
  Histogram2DDeltaEnergyWithARM();
  ~Histogram2DDeltaEnergyWithARM() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  
private:
  const EventReconstruction* eventReconstruction_;
  const anlgeant4::InitialInformation* initialInfo_;

  TH2* hist_all_;
  std::vector<TH2*> hist_vec_;
  TH2* hist_compton_all_;
  std::vector<TH2*> hist_compton_vec_;

  int numEnergyBins_;
  double energy0_;
  double energy1_;
  int numARMBins_;
  double arm0_;
  double arm1_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_Histogram2DDeltaEnergyWithARM_H */
