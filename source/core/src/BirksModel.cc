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

#include "BirksModel.hh"
namespace comptonsoft {
BirksModel::BirksModel() : VLArRecombinationModel("Birks Model") {
  kOverRho_ = k_ / Rho();
}
BirksModel::BirksModel(const std::map<std::string, double> &params) : VLArRecombinationModel("Birks Model", params) {
  auto it = params.find("Ab");
  if (it != params.end()) {
    Ab_ = it->second;
  }
  it = params.find("k");
  if (it != params.end()) {
    k_ = it->second * (CLHEP::kilovolt / CLHEP::cm3 * CLHEP::g / CLHEP::MeV);
  }
  kOverRho_ = k_ / Rho();
}
double BirksModel::electronLet(double let, double electricField) const {
  const double kOverRhoE = kOverRho_ / electricField;
  return Ab_ * (let / (1 + kOverRhoE * let));
}
void BirksModel::printInfo(std::ostream &os) const {
  VLArRecombinationModel::printInfo(os);
  os << "  Birks Constant (Ab): " << Ab_ << '\n'
     << "  k: " << k_ / (CLHEP::volt * CLHEP::g / CLHEP::cm3 / CLHEP::MeV) << " (Vg/cm3)/MeV\n"
     << "  k/Rho: " << kOverRho_ / (CLHEP::volt / CLHEP::MeV) << " (V/MeV)\n";
}
} /* namespace comptonsoft */