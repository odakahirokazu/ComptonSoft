/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato                                       *
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

#include "SpenvisIsotropicPrimaryGen.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

using namespace anl;
using namespace comptonsoft;

SpenvisIsotropicPrimaryGen::SpenvisIsotropicPrimaryGen()
  : m_ReadSPENVIS(0),
    m_SpenvisFileName("spenvis_triProtonDif.txt")
{
  add_alias("SpenvisIsotropicPrimaryGen");
}


ANLStatus SpenvisIsotropicPrimaryGen::mod_startup()
{
  IsotropicPrimaryGen::mod_startup();
  disableDefaultEnergyInput();
  register_parameter(&m_SpenvisFileName, "filename");
  
  return AS_OK;
}


ANLStatus SpenvisIsotropicPrimaryGen::mod_init()
{
  IsotropicPrimaryGen::mod_init();
  m_ReadSPENVIS = new ReadSPENVIS(m_SpenvisFileName);

  return AS_OK;
}


G4double SpenvisIsotropicPrimaryGen::sampleEnergy()
{ 
  return m_ReadSPENVIS->GetParticleEnergy() * MeV;
}
