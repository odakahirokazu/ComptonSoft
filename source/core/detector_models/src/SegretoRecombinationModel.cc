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

#include "SegretoRecombinationModel.hh"
#include <algorithm>
#include <cmath>

namespace comptonsoft {
SegretoRecombinationModel::SegretoRecombinationModel() : VLArRecombinationModel("Segreto Model") {
}

SegretoRecombinationModel::SegretoRecombinationModel(const std::map<std::string, double>& params)
  : VLArRecombinationModel("Segreto Model", params)
{
  auto it = params.find("k");
  if (it != params.end()) {
    k_ = it->second * CLHEP::volt;
  }
  it = params.find("Alpha");
  if (it != params.end()) {
    alpha_ = it->second * (CLHEP::eV * CLHEP::eV / CLHEP::cm);
  }
  it = params.find("Beta");
  if (it != params.end()) {
    beta_ = it->second * (CLHEP::eV / CLHEP::cm);
  }
  it = params.find("Gamma");
  if (it != params.end()) {
    gamma_ = it->second / CLHEP::cm;
  }
}

RecombinationResult SegretoRecombinationModel::calculateRecombination(double energy, double dx, double electricField,
                                                                      double /*dedx*/)
{
  // pass the energy deposit in place of dE/dx so that recombinationRate() receives it
  return VLArRecombinationModel::calculateRecombination(energy, dx, electricField, energy);
}

double SegretoRecombinationModel::recombinationRate(double edep, double electricField) const
{
  if (edep <= 0.0 || electricField <= 0.0) {
    return 0.0;
  }
  const double denominator = electricField + k_ * beta_ / Wion();
  const double z = edep * denominator / (k_ * alpha_ / Wion());
  // 1 - ln(1+z)/z; series expansion for small z to avoid cancellation
  const double trackFactor = (z < 1.0e-4) ? z * (0.5 - z * (1.0 / 3.0 - 0.25 * z)) : 1.0 - std::log1p(z) / z;
  const double p = (electricField + k_ * gamma_) / denominator * trackFactor;
  return std::clamp(p, 0.0, 1.0);
}

void SegretoRecombinationModel::printInfo(std::ostream& os) const
{
  VLArRecombinationModel::printInfo(os);
  os << "  k: " << k_ / CLHEP::volt << " V\n"
     << "  Alpha: " << alpha_ / (CLHEP::eV * CLHEP::eV / CLHEP::cm) << " eV^2/cm\n"
     << "  Beta: " << beta_ / (CLHEP::eV / CLHEP::cm) << " eV/cm\n"
     << "  Gamma: " << gamma_ * CLHEP::cm << " 1/cm\n";
}
} /* namespace comptonsoft */
