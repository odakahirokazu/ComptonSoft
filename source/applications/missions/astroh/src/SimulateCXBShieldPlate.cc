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

#include "SimulateCXBShieldPlate.hh"

#include <fstream>
#include <cmath>
#include "TFile.h"
#include "TH2.h"
#include "TGraph.h"
#include "TRandom3.h"
#include "AstroUnits.hh"
#include "InitialInformation.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

SimulateCXBShieldPlate::SimulateCXBShieldPlate()
  : m_ShieldDensity(1.0*unit::g/unit::cm3),
    m_ShieldThickness(0.0),
    m_ShieldHeight(0.0),
    m_ShieldFillingFraction(1.0),
    m_RandomGen(new TRandom3)
{
}

ANLStatus SimulateCXBShieldPlate::mod_define()
{
  define_parameter("density", &mod_class::m_ShieldDensity, unit::g/unit::cm3, "g/cm3");
  define_parameter("height", &mod_class::m_ShieldHeight, unit::cm, "cm");
  define_parameter("thickness", &mod_class::m_ShieldThickness, unit::cm, "cm");
  define_parameter("filling_fraction", &mod_class::m_ShieldFillingFraction);
  define_parameter("cross_section_file", &mod_class::m_CSFilename);
  define_parameter("position_file", &mod_class::m_PositionFilename);

  return AS_OK;
}

ANLStatus SimulateCXBShieldPlate::mod_initialize()
{
  get_module_IFNC("InitialInformation", &m_InitialInfo);

  {
    std::ifstream fin(m_CSFilename);
    if (fin) {
      std::vector<double> xs;
      std::vector<double> ys;
      double c1(0.0), c2(0.0);
      while (fin >> c1 >> c2) {
        const double energy = c1 * unit::MeV;
        const double alpha = (c2 * unit::cm2/unit::g) * m_ShieldDensity;
        xs.push_back(energy);
        ys.push_back(alpha);
      }

      m_CS.reset(new TGraph(xs.size(), &xs[0], &ys[0]));
    }
    else {
      std::cout << "Cannot open CS file: " << m_CSFilename << std::endl;
      return AS_QUIT_ERROR;
    }
  }

  m_PositionFile.reset(new TFile(m_PositionFilename.c_str()));
  m_ShieldDistribution = (TH2*)m_PositionFile->Get("shield");

  return AS_OK;
}

ANLStatus SimulateCXBShieldPlate::mod_analyze()
{
  const G4ThreeVector dir0 = m_InitialInfo->initial_direction();
  const G4ThreeVector pos0 = m_InitialInfo->initial_position();

  if (dir0.z() >= 0.0) { // check if the photon goes down
    return AS_OK;
  }

  const double xi = (m_ShieldHeight-pos0.z())/dir0.z();
  const G4ThreeVector pos1 = pos0 + xi*dir0; // position on the shield plane
  const int binX = m_ShieldDistribution->GetXaxis()->FindBin(pos1.x()/unit::cm);
  const int binY = m_ShieldDistribution->GetYaxis()->FindBin(pos1.y()/unit::cm);
  if (binX <= 0 || m_ShieldDistribution->GetXaxis()->GetNbins() < binX) {
    return AS_OK;
  }
  if (binY <= 0 || m_ShieldDistribution->GetYaxis()->GetNbins() < binY) {
    return AS_OK;
  }

  const bool onShield = (m_ShieldDistribution->GetBinContent(binX, binY) > 0.0);

  if (onShield) {
    if (m_RandomGen->Uniform(1.0) > m_ShieldFillingFraction) {
      return AS_OK;
    }

    const double weight0 = m_InitialInfo->weight();
    const double energy0 = m_InitialInfo->initial_energy();
    const double alpha = m_CS->Eval(energy0);
    if (alpha<=0.0) {
      return AS_OK;
    }

    const double cosTheta = -dir0.z();
    const double shieldLength = m_ShieldThickness/cosTheta;
    const double attenuation = std::exp(-alpha*shieldLength);
    const double weight1 = weight0 * attenuation;
    m_InitialInfo->set_weight(weight1);
  }

  return AS_OK;
}

ANLStatus SimulateCXBShieldPlate::mod_finalize()
{
  m_PositionFile->Close();
  return AS_OK;
}

} /* namespace comptonsoft */
