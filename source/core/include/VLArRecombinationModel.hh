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
#include <string>

namespace comptonsoft {
/**
 * A base class for LAr recombination model.
 * @author Shota Arai
 * @date 
 */
class VLArRecombinationModel {
public:
  VLArRecombinationModel() = default;
  VLArRecombinationModel(const std::string &name) : name_(name) {}
  virtual ~VLArRecombinationModel() = default;

  // Pure virtual function to calculate recombination energy
  virtual double electronLet(double let, double electricField) const = 0;
  virtual double lightYield(double let, double electricField) const {
    return let / Wexc();
  };
  double electronYield(double let, double electricField) const {
    return electronLet(let, electricField) / Wion();
  }
  std::string name() const { return name_; }
  virtual void printInfo(std::ostream &os) const {
    os << "Recombination Model\n"
       << "  Name: " << name() << '\n'
       << "  Wion: " << Wion_ << " keV\n"
       << "  Wexc: " << Wexc_ << " keV\n"
       << "  Density: " << rho_ << " g/cm3\n";
  }

protected:
  double Wion() const { return Wion_; } /// Ionization energy of LAr in keV
  double Wexc() const { return Wexc_; } /// Excitation energy of LAr in keV
  double Rho() const { return rho_; } /// Density of LAr in g/cm^3
private:
  std::string name_ = "VLArRecombinationModel";
  static constexpr double Wion_ = 23.6 * CLHEP::eV / CLHEP::keV; // in keV
  static constexpr double Wexc_ = 19.5 * CLHEP::eV / CLHEP::keV; // in eV
  static constexpr double rho_ = 1.39; // in g/cm^3
};
} // namespace comptonsoft
#endif //COMPTONSOFT_VLArRecombinationModel_H