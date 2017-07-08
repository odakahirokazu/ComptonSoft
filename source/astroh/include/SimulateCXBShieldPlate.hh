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

#ifndef COMPTONSOFT_SimulateCXBShieldPlate_H
#define COMPTONSOFT_SimulateCXBShieldPlate_H 1

#include "VCSModule.hh"
#include <memory>

class TH2;
class TGraph;
class TRandom;
namespace anlgeant4 { class InitialInformation; }

namespace comptonsoft {

class CSHitCollection;

/**
 *
 * @author Hirokazu Odaka
 * @date 2016-10-06
 */
class SimulateCXBShieldPlate : public VCSModule
{
  DEFINE_ANL_MODULE(SimulateCXBShieldPlate, 1.0);
public:
  SimulateCXBShieldPlate();  
  ~SimulateCXBShieldPlate() = default;

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;
  anl::ANLStatus mod_finalize() override;
  
private:
  double m_ShieldDensity;
  double m_ShieldThickness;
  double m_ShieldHeight;
  double m_ShieldFillingFraction;
  std::string m_CSFilename;
  std::string m_PositionFilename;
  
  anlgeant4::InitialInformation* m_InitialInfo = nullptr;
  std::unique_ptr<TFile> m_PositionFile;
  std::unique_ptr<TGraph> m_CS;
  TH2* m_ShieldDistribution = nullptr;
  std::unique_ptr<TRandom> m_RandomGen;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SimulateCXBShieldPlate_H */
