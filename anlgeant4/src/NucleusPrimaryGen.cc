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

#include "G4Ions.hh"
#include "G4IonTable.hh"
#include "G4VIsotopeTable.hh"
#include "AstroUnits.hh"

using namespace anlnext;

namespace anlgeant4
{

NucleusPrimaryGen::NucleusPrimaryGen()
  : m_Position0(0.0, 0.0, 0.0),
    m_RIZ(55), m_RIA(137), m_RIEnergy(0.0), m_RIFloatingLevel(0)
{
  add_alias("NucleusPrimaryGen");
}

NucleusPrimaryGen::~NucleusPrimaryGen() = default;

ANLStatus NucleusPrimaryGen::mod_define()
{
  BasicPrimaryGen::mod_define();

  unregister_parameter("particle");
  setParticleName("");
  register_parameter(&m_Position0, "position", unit::cm, "cm");
  set_parameter_description("Position of the source.");
  register_parameter(&m_RIZ, "atomic_number");
  set_parameter_description("Atomic number of the radioactive isotope.");
  register_parameter(&m_RIA, "mass_number");
  set_parameter_description("Mass number of the radioactive isotope.");
  register_parameter(&m_RIEnergy, "energy", unit::keV, "keV");
  set_parameter_description("Excitation energy of the radioactive isotope. A value of 0 means the ground state of the nucleus.");
  register_parameter(&m_RIFloatingLevel, "floating_level");
  set_parameter_description("Index specifying a floating level. A value of 0 means a determined level.");
  
  return AS_OK;
}

ANLStatus NucleusPrimaryGen::mod_begin_run()
{
  BasicPrimaryGen::mod_begin_run();
  
  G4IonTable* ionTable = static_cast<G4IonTable*>(G4ParticleTable::GetParticleTable()->GetIonTable());
  G4ParticleDefinition* particle_base =
    ionTable->GetIon(m_RIZ, m_RIA, m_RIEnergy, G4Ions::FloatLevelBase(m_RIFloatingLevel));
  G4Ions* particle = dynamic_cast<G4Ions*>(particle_base);
  if (particle == nullptr) {
    std::cout << "The particle can not be converted into G4Ions." << std::endl;
    return AS_QUIT_ERROR;
  }
  
  setDefinition(particle);
  
  G4double energy(0.0);
  G4ThreeVector direction(0.0, 0.0, 0.0);
  setPrimary(m_Position0, energy, direction);
  
  std::cout << "------------------------------ \n"
            << "  RI Information \n"
            << "    Name:           " << particle->GetParticleName()        << '\n'
            << "    Type:           " << particle->GetParticleType()        << '\n'
            << "    Z:              " << particle->GetAtomicNumber()        << '\n'
            << "    A:              " << particle->GetAtomicMass()          << '\n'
            << "    Floating level: " << particle->GetFloatLevelBaseIndex() << '\n'
            << "    life time:      " << particle->GetPDGLifeTime()/unit::s  << " second\n"
            << "------------------------------ \n"
            << std::endl;
  
  return AS_OK;
}

void NucleusPrimaryGen::makePrimarySetting()
{
}

} /* namespace anlgeant4 */
