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

#ifndef COMPTONSOFT_SegretoRecombinationModel_H
#define COMPTONSOFT_SegretoRecombinationModel_H 1
#include "AstroUnits.hh"
#include "VLArRecombinationModel.hh"

namespace comptonsoft {
/**
 * Segreto's recombination model for electrons in LAr.
 * The surviving rate depends on the total energy E of the electron (cluster) instead of dE/dx:
 *   R(E, F) = (F + k*gamma) / (F + k*beta/W) * (1 - ln(1+z)/z),
 *   z = E * (F + k*beta/W) / (k*alpha/W)
 * This model is intended to be applied to clustered hits, so the energy deposit is used and dE/dx is ignored.
 *
 * @author Shota Arai
 * @date 2026-10-22
 */
class SegretoRecombinationModel : public VLArRecombinationModel
{
public:
  SegretoRecombinationModel();
  SegretoRecombinationModel(const std::map<std::string, double>& params);
  virtual ~SegretoRecombinationModel() = default;

  /**
   * @brief Calculation of recombination using the energy deposit only (dx and dedx are ignored)
   */
  RecombinationResult calculateRecombination(double energy, double dx, double electricField,
                                             double dedx = -1) override;

  /**
   * @param edep total energy deposit of the cluster (not dE/dx)
   */
  double recombinationRate(double edep, double electricField) const override;

  void printInfo(std::ostream& os) const override;

private:
  // Constants of Segreto's model; same values as NanoGRAMSChargeToEnergySpline
  double k_ = 3.7e-3 * CLHEP::volt;                               // in V
  double alpha_ = 0.227e12 * (CLHEP::eV * CLHEP::eV / CLHEP::cm); // in eV^2/cm
  double beta_ = 1.7e6 * (CLHEP::eV / CLHEP::cm);                 // in eV/cm
  double gamma_ = 2.8e4 / CLHEP::cm;                              // in 1/cm
};
} // namespace comptonsoft
#endif // COMPTONSOFT_SegretoRecombinationModel_H
