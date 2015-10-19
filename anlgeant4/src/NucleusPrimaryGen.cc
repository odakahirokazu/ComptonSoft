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

#include "NucleusPrimaryGen.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Ions.hh"
#include "G4IonTable.hh"
#include "G4RIsotopeTable.hh"

using namespace anl;

namespace anlgeant4
{

NucleusPrimaryGen::NucleusPrimaryGen()
  : m_Position0(0.0, 0.0, 0.0),
    m_RIZ(55), m_RIA(137), m_RIEnergy(0.0)
{
  add_alias("NucleusPrimaryGen");
}

NucleusPrimaryGen::~NucleusPrimaryGen() = default;

ANLStatus NucleusPrimaryGen::mod_startup()
{
  BasicPrimaryGen::mod_startup();

  unregister_parameter("particle");
  setParticleName("");
  register_parameter(&m_Position0, "position", cm, "cm");
  set_parameter_description("Position of the source.");
  register_parameter(&m_RIZ, "atomic_number");
  set_parameter_description("Atomic number of the radioactive isotope.");
  register_parameter(&m_RIA, "mass_number");
  set_parameter_description("Mass number of the radioactive isotope.");
  register_parameter(&m_RIEnergy, "energy", keV, "keV");
  set_parameter_description("Excitation energy of the radioactive isotope. A value of 0 means the ground state of the nucleus.");
  
  return AS_OK;
}

ANLStatus NucleusPrimaryGen::mod_bgnrun()
{
  BasicPrimaryGen::mod_bgnrun();
  
  G4IonTable* ionTable = static_cast<G4IonTable*>(G4ParticleTable::GetParticleTable()->GetIonTable());
  G4ParticleDefinition* particle = 
    ionTable->GetIon(m_RIZ, m_RIA, m_RIEnergy);
  setDefinition(particle);
  
  G4double energy(0.0);
  G4ThreeVector direction(0.0, 0.0, 0.0);
  setPrimary(m_Position0, energy, direction);
  
  G4cout << "------------------------------ \n"
	 << "       RI Information \n"
	 << "      Name    " << particle->GetParticleName()       << '\n' 
	 << "      Type    " << particle->GetParticleType()       << '\n' 
	 << "       A      " << particle->GetAtomicMass()         << '\n' 
	 << "       Z      " << particle->GetAtomicNumber()       << '\n' 
	 << "   life time  " << particle->GetPDGLifeTime()/second << " sec\n"
	 << "------------------------------ \n"
	 << G4endl;
  
  return AS_OK;
}

} /* namespace anlgeant4 */
