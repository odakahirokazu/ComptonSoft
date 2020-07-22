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

#ifndef COMPTONSOFT_ComptonConstraints_H
#define COMPTONSOFT_ComptonConstraints_H 1

#include <vector>
#include "CSTypes.hh"
#include "DetectorHit_sptr.hh"

namespace comptonsoft {
namespace compton_constraints {

double differentialCrossSection(double incident_energy, double cos_theta);
double scatteredGammarayEnergy(double incident_energy, double cos_theta);
double scatteredGammarayEnergyFromEdep(double energy_deposit, double cos_theta);
double energyDeposit(double incident_energy, double cos_theta);

double cosThetaGeometry(const vector3_t& incident_direction, const vector3_t& scattering_direction);
double cosThetaKinematics(double incident_energy, double energy_deposit);

bool checkEdepIsPhysicallyAcceptable(double incident_energy,
                                     double energy_deposit,
                                     double delta_energy = 0.0);
bool checkScatteringAngle(const DetectorHit_sptr& hit1,
                          const DetectorHit_sptr& hit2,
                          const DetectorHit_sptr& hit3,
                          double incident_energy_at_secondpoint,
                          double delta_energy);

bool estimateEscapedEnergy(const std::vector<DetectorHit_sptr>& ordered_hits, double& escaped_energy);

} /* namespace compton_constraints */
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ComptonConstraints_H */
