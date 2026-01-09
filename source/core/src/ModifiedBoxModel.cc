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
#include "TMath.h"
#include "TRandom3.h"
namespace comptonsoft {
ModifiedBoxModel::ModifiedBoxModel() : VLArRecombinationModel("Modified Box Model") {
}
ModifiedBoxModel::ModifiedBoxModel(const std::map<std::string, double> &params) : VLArRecombinationModel("Modified Box Model", params) {
  auto it = params.find("Alpha");
  if (it != params.end()) {
    alpha = it->second;
  }
  it = params.find("Beta");
  if (it != params.end()) {
    beta = it->second * (CLHEP::kilovolt / CLHEP::cm3 * CLHEP::g / CLHEP::MeV);
  }
  betaOverRho_ = beta / Rho();
}
double ModifiedBoxModel::electronLet(double let, double electricField) const {
  const double betaOverRhoE = betaOverRho_ / electricField;
  const double yield = TMath::Log(betaOverRhoE * let + alpha) / betaOverRhoE;
  //std::cout << "ModifiedBoxModel::electronLet: let=" << let / (CLHEP::MeV / CLHEP::cm) << "MeV/cm, electricField=" << electricField / (CLHEP::kilovolt / CLHEP::cm) << " kV/cm, yield=" << yield / (CLHEP::MeV / CLHEP::cm) << "MeV/cm" << std::endl;
  if (RandomizeMode() == 0) {
    return yield;
  }
  else if (RandomizeMode() == 1) {
    // Binomial randomization
    const double nQuanta = let / Wexc(); // number of quanta
    //std::cout << "nQuanta: " << nQuanta << std::endl;
    int nQuantaWithFluctuations = TMath::Nint(gRandom->Gaus(nQuanta, TMath::Sqrt(TMath::Max(0.0, nQuanta * FanoFactor()))));
    if (nQuantaWithFluctuations < 0) {
      nQuantaWithFluctuations = 0;
    }
    //std::cout << "nQuantaWithFluctuations: " << nQuantaWithFluctuations << std::endl;
    double p = yield / let;
    if (p < 0.0) {
      return 0.0;
    }
    else if (p > 1.0) {
      return nQuantaWithFluctuations * Wion();
    }
    int nElectron = Binomial(nQuantaWithFluctuations, p);
    if (nElectron < 0.0) {
      nElectron = 0;
    }
    else if (nElectron > nQuantaWithFluctuations) {
      nElectron = nQuantaWithFluctuations;
    }
    else {
      nElectron = TMath::Nint(nElectron);
    }
    return nElectron * Wion();
  }
  else {
    // No randomization
    return yield;
  }
}
void ModifiedBoxModel::printInfo(std::ostream &os) const {
  VLArRecombinationModel::printInfo(os);
  os << "  Alpha: " << alpha << "\n"
     << "  Beta: " << beta / (CLHEP::kilovolt / CLHEP::cm3 * CLHEP::g / CLHEP::MeV) << " (kVg/cm3)/MeV\n"
     << "  Beta/Rho: " << betaOverRho_ / (CLHEP::kilovolt / CLHEP::MeV) << " kV/MeV\n";
}
} /* namespace comptonsoft */