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
}
double BirksModel::electronLet(double let, double electricField) const {
  const double kOverRhoE = kOverRho_ / electricField;
  return Ab_ * (let / (1 + kOverRhoE * let));
}
void BirksModel::printInfo(std::ostream &os) const {
  VLArRecombinationModel::printInfo(os);
  os << "  Birks Constant (Ab): " << Ab_ << '\n'
     << "  k: " << k << " (V/mm)(g/cm2)/MeV\n"
     << "  k/Rho: " << kOverRho_ << " (V/mm)/MeV\n";
}

} /* namespace comptonsoft */