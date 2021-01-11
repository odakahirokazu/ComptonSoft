/**
 * @file IsotopeDatabaseAccess.cc
 * @brief source file of class IsotopeDatabaseAccess
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#include "IsotopeDatabaseAccess.hh"

#include <iostream>
#include <cmath>
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4ParticleDefinition.hh"
#include "G4Ions.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

namespace comptonsoft
{

IsotopeDatabaseAccess::~IsotopeDatabaseAccess() = default;

void IsotopeDatabaseAccess::initialize_data()
{
  initialize_geant4();
}

void IsotopeDatabaseAccess::
retrive_isotope(int z, int a, double energy_in_keV, int floating_level)
{
  const double energy = energy_in_keV*CLHEP::keV;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  const G4ParticleDefinition* nucleus
    = particleTable->GetIonTable()->GetIon(z, a, energy, G4Ions::FloatLevelBase(floating_level));
  ions_ = dynamic_cast<const G4Ions*>(nucleus);
  if (ions_==nullptr) {
    std::cout << "Error: failed to obtain the specified ion.";
  }
  std::cout << "Ion: " << ions_->GetParticleName() << std::endl;
}

double IsotopeDatabaseAccess::get_lifetime() const
{
  return ions_->GetPDGLifeTime()/CLHEP::second;
}

double IsotopeDatabaseAccess::get_halflife() const
{
  return get_lifetime()*std::log(2.0);
}

} /* namespace comptonsoft */
