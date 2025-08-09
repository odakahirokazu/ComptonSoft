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

#include "AstroUnits.hh"
#include "VLArRecombinationModel.hh"

namespace comptonsoft {
class BirksModel: public VLArRecombinationModel {
public:
  BirksModel();
  virtual ~BirksModel() = default;
  virtual double electronLet(double let, double electricField) const override;
  void printInfo(std::ostream &os) const override;

private:
  // Constants for the Birks model
  // Reference: B. Birks, "The Theory and Practice of Scintillation Counting", 1964, Pergamon Press
  static constexpr double Ab_ = 0.800; // Dimensionless
  static constexpr double k = 0.0486 * (CLHEP::kilovolt / CLHEP::cm3 * CLHEP::g / CLHEP::MeV) / (CLHEP::volt / CLHEP::cm2 / CLHEP::mm * CLHEP::g / CLHEP::MeV); // in (V/mm)(g/cm2)/MeV
  const double kOverRho_ = k / Rho(); // in (V/mm)/MeV
};
} // namespace comptonsoft