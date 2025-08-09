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

#include "ModifiedBoxModel.hh"
namespace comptonsoft {
ModifiedBoxModel::ModifiedBoxModel() : VLArRecombinationModel("Modified Box Model") {
}
double ModifiedBoxModel::electronLet(double let, double electricField) const {
  const double betaOverRhoE = betaOverRho_ / electricField;
  return log(betaOverRhoE * let + alpha) / betaOverRhoE;
}
void ModifiedBoxModel::printInfo(std::ostream &os) const {
  VLArRecombinationModel::printInfo(os);
  os << "  Alpha: " << alpha << "\n"
     << "  Beta: " << beta << " (V/mm)(g/cm2)/MeV\n"
     << "  Beta/Rho: " << betaOverRho_ << " V/MeV\n";
}
} /* namespace comptonsoft */