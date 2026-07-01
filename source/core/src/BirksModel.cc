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
#include "TMath.h"
#include "TRandom3.h"
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
double BirksModel::electronDeDx(double dedx, double electricField) {
  const double kOverRhoE = kOverRho_ / electricField;
  const double new_dedx = Ab_ * (dedx / (1 + kOverRhoE * dedx));
  if (RandomizeMode() == 0) {
    return (new_dedx > 0.0 && dedx > new_dedx) ? new_dedx : 0.0;
  }
  else if (RandomizeMode() == 1) {
    // Binomial randomization
    const double nQuanta = dedx / Wion(); // number of quanta
    int nQuantaWithFluctuations = TMath::Nint(gRandom->Gaus(nQuanta, TMath::Sqrt(TMath::Max(0.0, nQuanta * FanoFactor()))));
    if (nQuantaWithFluctuations < 0) {
      nQuantaWithFluctuations = 0;
    }
    double p = new_dedx / dedx;
    if (p < 0.0) {
      return 0.0;
    }
    else if (p > 1.0) {
      return nQuantaWithFluctuations * Wion();
    }
    int nElectronInt = Binomial(static_cast<int>(nQuantaWithFluctuations), p);
    if (nElectronInt < 0.0) {
      nElectronInt = 0;
    }
    else if (nElectronInt > nQuantaWithFluctuations) {
      nElectronInt = nQuantaWithFluctuations;
    }
    return nElectronInt * Wion();
  }
  else {
    std::cerr << "BirksModel::electronDeDx: Unknown randomization mode: " << RandomizeMode() << std::endl;
    return (new_dedx > 0.0 && dedx > new_dedx) ? new_dedx : 0.0;
  }
}
void BirksModel::printInfo(std::ostream &os) const {
  VLArRecombinationModel::printInfo(os);
  os << "  Birks Constant (Ab): " << Ab_ << '\n'
     << "  k: " << k_ / (CLHEP::volt * CLHEP::g / CLHEP::cm3 / CLHEP::MeV) << " (Vg/cm3)/MeV\n"
     << "  k/Rho: " << kOverRho_ / (CLHEP::volt / CLHEP::MeV) << " (V/MeV)\n";
}
} /* namespace comptonsoft */