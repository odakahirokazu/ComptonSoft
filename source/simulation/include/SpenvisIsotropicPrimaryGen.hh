/*************************************************************************
 *                                                                       *
 * Copyright (c) 2012 Tamotsu Sato                                       *
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

#ifndef COMPTONSOFT_SpenvisIsotropicPrimaryGen_H
#define COMPTONSOFT_SpenvisIsotropicPrimaryGen_H 1

#include "IsotropicPrimaryGen.hh"
#include "ReadSPENVIS.hh"

namespace comptonsoft {


/**
 * ANLGeant4 primary generator according to a SPENVIS output spectrum.
 *
 * @author Tamotsu Sato
 * @date 2012-07-02 | Tamotsu Sato | ver 1.0: based on IsotropicPrimaryGen
 * @date 2012-07-05 | Tamotsu Sato | ver 1.1: inheritance IsotropicPrimaryGen / override sampleEnergy
 * @date 2012-07-06 | Hirokazu Odaka | ver 1.2: slight cleanup
 */
class SpenvisIsotropicPrimaryGen : public IsotropicPrimaryGen
{
  DEFINE_ANL_MODULE(SpenvisIsotropicPrimaryGen, 1.2);
public:
  SpenvisIsotropicPrimaryGen();
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  
  G4double sampleEnergy();
  
private:
  ReadSPENVIS* m_ReadSPENVIS;
  std::string m_SpenvisFileName;
};

}

#endif /* COMPTONSOFT_SpenvisIsotropicPrimaryGen_H */
