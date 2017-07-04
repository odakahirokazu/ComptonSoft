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
#include "AstroUnits.hh"
#include "TFile.h"
#include "TH1.h"
#include "TGraph.h"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

AHRadiationBackgroundPrimaryGen::AHRadiationBackgroundPrimaryGen()
  : m_Filename("trapped_proton_spectrum.root"),
    m_File(nullptr),
    m_Hist(nullptr)
{
  add_alias("AHRadiationBackgroundPrimaryGen");
}

AHRadiationBackgroundPrimaryGen::~AHRadiationBackgroundPrimaryGen() = default;

ANLStatus AHRadiationBackgroundPrimaryGen::mod_startup()
{
  anlgeant4::IsotropicPrimaryGen::mod_startup();
  disableDefaultEnergyInput();
  register_parameter(&m_Filename, "filename");
  set_parameter_description("ROOT file of background radiation spectrum.");
  return AS_OK;
}

ANLStatus AHRadiationBackgroundPrimaryGen::mod_init()
{
  anlgeant4::IsotropicPrimaryGen::mod_init();
  
  m_File.reset(new TFile(m_Filename.c_str()));
  if ( m_File->IsZombie() ) {
    std::cout << "Cannot open " << m_Filename << " ! " << std::endl;
    return AS_QUIT_ERR;
  }
  
  TGraph* graph = (TGraph*) m_File->Get("Graph");
  const int N = graph->GetN();
  
  double* x_array = graph->GetX(); // GeV
  std::vector<double> energies(N);
  for (int i=0; i<N; i++) {
    energies[i] = x_array[i] * unit::GeV;
  }

  m_Hist = new TH1D("spectrum", "spectrum", N-1, &energies[0]);
  double sum(0.0);
  std::cout << "** output spectral information of particles **" << std::endl;
  
  for (int bin=1; bin<=N; bin++) {
    const double energy = m_Hist->GetBinCenter(bin);
    const double differentialFlux = graph->Eval(energy/unit::GeV) * (1.0/unit::s/unit::m2/unit::sr/unit::GeV);
    const double deltaE = m_Hist->GetBinWidth(bin);
    const double flux = differentialFlux * deltaE;
    m_Hist->SetBinContent(bin, flux);
    sum += flux;
    std::cout << energy/unit::MeV << " [MeV] : " << differentialFlux/(1.0/unit::s/unit::cm2/unit::sr/unit::MeV) << " [#/s/cm2/sr/MeV] " << std::endl;
  }
  
  const double circleArea = Radius()*Radius()*CLHEP::pi;
  const double norm = sum * circleArea * (4.0*CLHEP::pi*CoveringFactor());
  std::cout << " -> Normalization factor: " << norm/(1.0/unit::s) << " particles/s\n" << std::endl;
  
  return AS_OK;
}

G4double AHRadiationBackgroundPrimaryGen::sampleEnergy()
{
  return m_Hist->GetRandom();
}

} /* namespace comptonsoft */
