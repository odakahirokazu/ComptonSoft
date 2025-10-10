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
#include <string>

namespace comptonsoft {
/**
 * A base class for LAr recombination model.
 * @author Shota Arai
 * @date 2025-08-09
 * @date 2025-10-10 | added parameterized constructor
 */
class VLArRecombinationModel {
public:
  VLArRecombinationModel();
  VLArRecombinationModel(const std::string &name, const std::map<std::string, double> &params);
  VLArRecombinationModel(const std::string &name);
  virtual ~VLArRecombinationModel();

  // Pure virtual function to calculate recombination energy
  virtual double electronLet(double let, double electricField) const = 0;
  virtual double lightYield(double let, double /*electricField*/) const {
    return let / Wexc();
  };
  double electronYield(double let, double electricField) const {
    return electronLet(let, electricField) / Wion();
  }
  std::string name() const { return name_; }
  virtual void printInfo(std::ostream &os) const;

protected:
  double Wion() const { return Wion_; } /// Ionization energy of LAr
  double Wexc() const { return Wexc_; } /// Excitation energy of LAr
  double Rho() const { return rho_; } /// Density of LAr
private:
  std::string name_ = "VLArRecombinationModel";
  double Wion_ = 23.6 * CLHEP::eV; // in eV
  double Wexc_ = 19.5 * CLHEP::eV; // in eV
  double rho_ = 1.39 * (CLHEP::g / CLHEP::cm3); // in g/cm^3
};
} // namespace comptonsoft
#endif //COMPTONSOFT_VLArRecombinationModel_H