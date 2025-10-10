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
/**
 * A Birks model for recombination in LArTPC.
 * @author Shota Arai
 * @date 2025-08-09
 * @date 2025-10-10 | added parameterized constructor
 */
class BirksModel: public VLArRecombinationModel {
public:
  BirksModel();
  BirksModel(const std::map<std::string, double> &params);
  virtual ~BirksModel() = default;
  virtual double electronLet(double let, double electricField) const override;
  void printInfo(std::ostream &os) const override;

private:
  // Constants for the Birks model
  // Reference: B. Birks, "The Theory and Practice of Scintillation Counting", 1964, Pergamon Press
  double Ab_ = 0.800; // Dimensionless
  double k_ = 0.0486 * (CLHEP::kilovolt / CLHEP::cm3 * CLHEP::g / CLHEP::MeV); // in (kV g/cm3)/MeV
  double kOverRho_ = k_ / Rho(); // in (Vg/cm3)/MeV
};
} // namespace comptonsoft