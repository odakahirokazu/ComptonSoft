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

#ifndef COMPTONSOFT_AHRadiationBackgroundPrimaryGen_H
#define COMPTONSOFT_AHRadiationBackgroundPrimaryGen_H 1

#include "IsotropicPrimaryGen.hh"
#include <memory>

class TFile;
class TH1D;


namespace comptonsoft {

/**
 * ANLGeant4 primary generator reading the ASTRO-H radiation-background files.
 *
 * @author Tamotsu Sato
 * @date 2013-04-11 | Tamotsu Sato | ver 1.0: based on IsotropicPrimaryGen
 * @date 2013-05-03 | Tamotsu Sato & Hirokazu Odaka | ver 1.1 bug fix: unit conversion
 * @date 2017-03-23 | Hirokazu Odaka | use unique_ptr for root File.
 * @date 2017-07-27 | Hirokazu Odaka | tweak.
 */
class AHRadiationBackgroundPrimaryGen : public anlgeant4::IsotropicPrimaryGen
{
  DEFINE_ANL_MODULE(AHRadiationBackgroundPrimaryGen, 4.1);
public:
  AHRadiationBackgroundPrimaryGen();
  ~AHRadiationBackgroundPrimaryGen();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  
  G4double sampleEnergy() override;

private:
  std::string m_Filename;
  std::unique_ptr<TFile> m_File;
  TH1D* m_Hist;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AHRadiationBackgroundPrimaryGen_H */
