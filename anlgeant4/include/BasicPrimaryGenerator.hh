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

#ifndef ANLGEANT4_BasicPrimaryGenerator_H
#define ANLGEANT4_BasicPrimaryGenerator_H 1

#include "VANLPrimaryGenerator.hh"

#include <mutex>

#include "InitialInformation.hh"
#include "BasicPrimaryGeneratorAction.hh"

namespace anlgeant4 {

class VANLGeometry;

/**
 * ANLGeant4 PrimaryGen module
 *
 * @author Hirokazu Odaka
 * @date 2011-04-11
 * @date 2012-07-04 | Hirokazu Odaka | sampleEnergy(), printSpectralInfo() as virtual
 * @date 2012-07-10 | Hirokazu Odaka | virtual methods: sampleDirection(), samplePosition()
 * @date 2014-12-15 | Hirokazu Odaka | histogram spectral distribution
 * @date 2017-07-27 | Hirokazu Odaka | this can transfer its setting function to the generator action, introducing makePrimarySetting().
 * @date 2017-07-03 | 4.2 | Hirokazu Odaka | length unit is fixed to cm
 * @date 2020-04-13 | 5.0 | Hirokazu Odaka | remove polarization mode
 * @date 2024-03-08 | 6.0 | Hirokazu Odaka | nucleus
 * @date 2026-06-20 | 7.0 | Hirokazu Odaka | Geant4-MT
 */
class BasicPrimaryGenerator : public VANLPrimaryGenerator
{
  DEFINE_ANL_MODULE(BasicPrimaryGenerator, 7.0);

public:
  enum class SpectralShape {
    undefined, mono, powerlaw, gaussian, blackbody, histogram, user,
  };

public:
  BasicPrimaryGenerator();
  ~BasicPrimaryGenerator();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;

  G4VUserPrimaryGeneratorAction* create() override;

  const G4ParticleDefinition* particle_definition() const;
  virtual PrimarySetting make_primary_setting() const = 0;
  void confirm_primary_setting(int event_id, const PrimarySetting& primary_info);

protected:
  void set_particle_definition(G4ParticleDefinition* particle_definition);
  void set_particle_definition_by_name(const std::string& name);
  void set_nucleus_definition(int atomic_number, int mass_number, double excitation_energy, int floating_level);

  void set_energy_distribution(SpectralShape v, const std::string& name)
  {
    energy_distribution_ = v;
    energy_distribution_name_ = name;
  }

  void enable_powerlaw_input();
  void enable_gaussian_input();
  void enable_blackbody_input();
  void enable_histogram_input();
  void disable_default_energy_input();

  void build_spectrum_photon_integral();

  virtual void print_spectral_info() const;

  /**
   * Sample a value of energy from energy distribution of the primaries.
   * Since this methods is virtual, you can override its implimentation.
   * By default, it samples an energy from a power-law distribution
   * specified by registered parameters.
   *
   * @return sampled energy
   */
  virtual double sample_energy() const;
  double sample_from_powerlaw(double gamma, double e0, double e1) const;
  double sample_from_powerlaw() const;
  double sample_from_gaussian(double mean, double sigma) const;
  double sample_from_gaussian() const;
  double sample_from_blackbody(double kT, double upper_limit_factor) const;
  double sample_from_blackbody() const;
  double sample_from_histogram() const;

  virtual G4ThreeVector sample_direction() const { return G4ThreeVector(0.0, 0.0, -1.0); }
  virtual G4ThreeVector sample_position() const { return G4ThreeVector(0.0, 0.0, 0.0); }

  G4ThreeVector unpolarized_vector(G4ThreeVector direction) const;

  void set_spectrum_energy(const std::vector<double>& v) { spectrum_energy_ = v; }
  const std::vector<double>& spectrum_energy() const { return spectrum_energy_; }
  void set_spectrum_photons(const std::vector<double>& v) { spectrum_photons_ = v; }
  const std::vector<double>& spectrum_photons() const { return spectrum_photons_; }

  std::string get_energy_distribution_name() const { return energy_distribution_name_; }
  SpectralShape get_energy_distribution() const { return  energy_distribution_; }
  double get_energy_min() const { return energy_min_; }
  double get_energy_max() const { return energy_max_; }
  double get_photon_index() const { return photon_index_; }
  double get_energy_mean() const { return energy_mean_; }
  double get_energy_sigma() const { return energy_sigma_; }
  double get_energy_kT() const { return kT_; }

  /**
   * statistical information
   */
  int number() const { return number_; }
  double total_energy() const { return total_energy_; }

  void set_real_time(double v) { real_time_ = v; }
  double real_time() const { return real_time_; }

private:
  void store_initial_condition(int event_id, const PrimarySetting& primary_info);

private:
  const anlgeant4::VANLGeometry* geometry_ = nullptr;
  anlgeant4::InitialInformation* initial_info_ = nullptr;

  /*
   * statistical information
   */
  int number_;
  double total_energy_;
  double real_time_;

  /*
   * global particle information
   */
  G4ParticleDefinition* particle_definition_ = nullptr;
  std::string particle_name_;

  // nucleus
  int nucleus_atomic_number_;
  int nucleus_mass_number_;
  double nucleus_excitation_energy_;
  int nucleus_floating_level_;

  /*
   * properties for the energy distribution
   */
  std::string energy_distribution_name_;
  SpectralShape energy_distribution_;
  double energy_min_;
  double energy_max_;
  // power-law distribution
  double photon_index_;
  // Gaussin distribution
  double energy_mean_;
  double energy_sigma_;
  // black-body distribution
  double kT_;
  // histogram
  std::vector<double> spectrum_energy_;
  std::vector<double> spectrum_photons_;
  std::vector<double> spectrum_photon_integral_;

  /**
   * for statistical info collection
   */
  std::mutex mutex_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_BasicPrimaryGenerator_H */
