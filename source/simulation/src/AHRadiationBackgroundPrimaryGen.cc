/*************************************************************************
 *                                                                       *
 * Copyright (c) 2013 Tamotsu Sato, Hirokazu Odaka                       *
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

#include "AHRadiationBackgroundPrimaryGen.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "TGraph.h"

using namespace anl;
using namespace comptonsoft;

AHRadiationBackgroundPrimaryGen::AHRadiationBackgroundPrimaryGen()
  : m_Filename("trapped_proton_spectrum.root"),
    m_File(0), m_Hist(0)
{
  add_alias("AHRadiationBackgroundPrimaryGen");
}


ANLStatus AHRadiationBackgroundPrimaryGen::mod_startup()
{
  IsotropicPrimaryGen::mod_startup();
  disableDefaultEnergyInput();
  register_parameter(&m_Filename, "File name");
  set_parameter_description("ROOT file of background radiation spectrum.");
  return AS_OK;
}


ANLStatus AHRadiationBackgroundPrimaryGen::mod_init()
{
  IsotropicPrimaryGen::mod_init();
  
  m_File = new TFile(m_Filename.c_str());
  if ( m_File->IsZombie() ) {
    G4cout << "Cannot open " << m_Filename << " ! " << G4endl;
    return AS_QUIT_ERR;
  }
  
  TGraph* graph = (TGraph*) m_File->Get("Graph");
  const int N = graph->GetN();
  
  double* x_array = graph->GetX(); // GeV
  std::vector<double> energies(N);
  for (int i=0; i<N; i++) {
    energies[i] = x_array[i] * GeV;
  }

  m_Hist = new TH1D("spectrum", "spectrum", N-1, &energies[0]);
  double sum(0.0);
  G4cout << "** output spectral information of particles **" << G4endl;
  
  for (int bin=1; bin<=N; bin++) {
    double energy = m_Hist->GetBinCenter(bin);
    double differentialFlux = graph->Eval(energy/GeV) * (1.0/s/m2/sr/GeV);
    double deltaE = m_Hist->GetBinWidth(bin);
    double flux = differentialFlux * deltaE;
    m_Hist->SetBinContent(bin, flux);
    sum += flux;
    G4cout << energy/MeV << " [MeV] : " << differentialFlux/(1.0/s/cm2/sr/MeV) << " [#/s/cm2/sr/MeV] " << G4endl;
  }
  
  double circleArea = Radius()*Radius()*pi;
  double norm = sum * circleArea * (4.0*pi*CoveringFactor());
  G4cout << " -> Normalization factor: " << norm/(1.0/s) << " particles/s\n" << G4endl;
  
  return AS_OK;
}


G4double AHRadiationBackgroundPrimaryGen::sampleEnergy()
{
  return m_Hist->GetRandom();
}
