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

#ifndef COMPTONSOFT_VLArRecombinationModel_H
#define COMPTONSOFT_VLArRecombinationModel_H 1

#include "AstroUnits.hh"
#include <iostream>
#include <map>
#include <random>
#include <string>

namespace comptonsoft {
struct RecombinationResult {
  int photonYield;
  int electronYield;
  double electronEnergy;
  double recombinationSurvivingRate;
  int totalQuanta;
  };
/**
 * A base class for LAr recombination model.
 * @author Shota Arai
 * @date 2025-08-09
 * @date 2025-10-10 | added parameterized constructor
 * @date 2026-02-21 | Change random generator to TRandom3 instead of
 * std::mt19937
 */


class VLArRecombinationModel {
public:
  VLArRecombinationModel();
  VLArRecombinationModel(const std::string &name, const std::map<std::string, double> &params);
  VLArRecombinationModel(const std::string &name);
  virtual ~VLArRecombinationModel();

  // Pure virtual function to calculate recombination energy
  /**
    * @brief Calculation of Recombination
    */
  virtual RecombinationResult calculateRecombination(double energy, double dx, double electricField, double dedx = -1);
  
  virtual double recombinationRate(double dedx, double electricField) const = 0;

  double lightYield(double edep, double recombination_factor) const {
    if (edep <= 0.0) {
      return 0.0;
    }
    const double total_quanta = edep / Wexc();
    const double light = total_quanta - recombination_factor * edep / Wion();
    return std::max(light, 0.0);
  }
  
  std::string name() const { return name_; }
  virtual void printInfo(std::ostream &os) const;
  int RandomizeMode() const { return randomizeMode_; }
  void setRandomizeMode(int mode) { randomizeMode_ = mode; }
  double FanoFactor() const { return fanoFactor_; }
  void setFanoFactor(double fano) { fanoFactor_ = fano; }

  double Wion() const { return Wion_; } /// Ionization energy of LAr
  double Wexc() const { return Wexc_; } /// Excitation energy of LAr
  double Rho() const { return rho_; }   /// Density of LAr
  double IonExitonRatio() const { return ionExitonRatio_; }

private:
  void calAlpha() {
     ionExitonRatio_ = Wion_ / Wexc_ - 1.0;
  }
  std::string name_ = "VLArRecombinationModel";
  double Wion_ = 23.6 * CLHEP::eV; // in eV
  double Wexc_ = 19.6 * CLHEP::eV; // in eV
  double ionExitonRatio_ = Wion_ / Wexc_ - 1.0;
  double rho_ = 1.39 * (CLHEP::g / CLHEP::cm3); // in g/cm^3
  int randomizeMode_ = 0; // 0: no randomization, 1: Binomial
  double fanoFactor_ = 0.107; // Fano factor for LAr // Referece: Bonivento, W. M. and Terranova, F., "The science and technology of liquid argon detectors", 2024,
  std::mt19937 rng_;
  uint32_t seed_ = 0;

protected:
  int Binomial(int n, double p);
};
} // namespace comptonsoft
#endif //COMPTONSOFT_VLArRecombinationModel_H