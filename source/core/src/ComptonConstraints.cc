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

#include "ComptonConstraints.hh"

#include <cmath>
#include "AstroUnits.hh"
#include "DetectorHit.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {
namespace compton_constraints {

double differentialCrossSection(const double incident_energy, const double cos_theta)
{
  const double scattered_energy = scatteredGammarayEnergy(incident_energy, cos_theta);
  const double ratio_energy = scattered_energy/incident_energy;
  const double sin_theta_square = 1 - cos_theta * cos_theta;
  return 0.5 * CLHEP::classic_electr_radius * CLHEP::classic_electr_radius * 
    ratio_energy * ratio_energy * (ratio_energy + 1.0/ratio_energy - sin_theta_square);
}

double ComptonCrossSection(double incident_energy)
{
  const double x = incident_energy/CLHEP::electron_mass_c2;
  return 0.75 * 8 * CLHEP::pi / 3 * 
      CLHEP::classic_electr_radius * CLHEP::classic_electr_radius * 
      ( (1+x)/std::pow(x, 3) * ( 2*x*(1+x)/(1+2*x) - std::log(1 + 2*x) ) 
        + std::log(1 + 2*x)/2/x - (1 + 3*x)/pow(1 + 2*x, 2) );
}

double normalized_differentialCrossSection(double incident_energy, double cos_theta)
{
    return differentialCrossSection(incident_energy, cos_theta) / ComptonCrossSection(incident_energy);
}

double scatteredGammarayEnergy(const double incident_energy, const double cos_theta)
{
  return incident_energy/(1 + incident_energy/CLHEP::electron_mass_c2 * (1 - cos_theta));
}

double scatteredGammarayEnergyFromEdep(const double energy_deposit, const double cos_theta)
{
  return -0.5 * energy_deposit
    + std::sqrt( std::pow(energy_deposit, 2)/4.0 
                 + energy_deposit * CLHEP::electron_mass_c2/(1 - cos_theta) );
}

double energyDeposit(const double incident_energy, const double cos_theta)
{
  return incident_energy - scatteredGammarayEnergy(incident_energy, cos_theta);
}

double cosThetaGeometry(const vector3_t& incident_direction, const vector3_t& scattering_direction)
{
  return incident_direction.unit().dot(scattering_direction.unit());
}

double cosThetaKinematics(const double incident_energy, const double energy_deposit)
{
  return 1.0 - CLHEP::electron_mass_c2/(incident_energy - energy_deposit)
    + CLHEP::electron_mass_c2/incident_energy;
}

bool checkEdepIsPhysicallyAcceptable(const double incident_energy, 
                                     const double energy_deposit, 
                                     const double delta_energy)
{
  const double max_energy_deposit = 2*std::pow(incident_energy, 2)/(2*incident_energy + CLHEP::electron_mass_c2);
  if (-delta_energy < energy_deposit && energy_deposit < max_energy_deposit + delta_energy) {
    return true;
  }
  return false;
}

bool checkScatteringAngle(const DetectorHit_sptr& hit1, 
                          const DetectorHit_sptr& hit2, 
                          const DetectorHit_sptr& hit3, 
                          const double incident_energy_at_secondpoint,
                          const double delta_energy)
{
  const vector3_t incident_direction_at_secondpoint = hit2->Position() - hit1->Position();
  const vector3_t scattering_direction_at_secondpoint = hit3->Position() - hit2->Position();
  const double cos_theta_geom = cosThetaGeometry(incident_direction_at_secondpoint, scattering_direction_at_secondpoint);
  const double edep_geom = energyDeposit(incident_energy_at_secondpoint, cos_theta_geom);
  
  bool is_acceptable = false;
  if (std::abs(edep_geom - hit2->Energy()) < delta_energy) {
    is_acceptable = true;
  }
  return is_acceptable;
}

bool estimateEscapedEnergy(const std::vector<DetectorHit_sptr>& ordered_hits,
                           double& escaped_energy)
{
  const int num_hits = ordered_hits.size();
  if (num_hits < 3) {
    return false;
  }

  const vector3_t incident_direction_at_lastsecondpoint = ordered_hits[num_hits - 2]->Position() - ordered_hits[num_hits - 3]->Position();
  const vector3_t scattering_direction_at_lastsecondpoint = ordered_hits[num_hits - 1]->Position() - ordered_hits[num_hits - 2]->Position();

  const double cos_theta_geom = cosThetaGeometry(incident_direction_at_lastsecondpoint, scattering_direction_at_lastsecondpoint);
  const double scattered_energy_at_lasesecondpoint = scatteredGammarayEnergyFromEdep(ordered_hits[num_hits-2]->Energy(), cos_theta_geom);

  if (scattered_energy_at_lasesecondpoint > 0 && scattered_energy_at_lasesecondpoint > ordered_hits[num_hits - 1]->Energy()) {
    escaped_energy = scattered_energy_at_lasesecondpoint - ordered_hits[num_hits - 1]->Energy();
    return true;
  }

  return false;
}

bool estimateAveragedEscapedEnergy(const std::vector<DetectorHit_sptr>& ordered_hits,
                                   double& escaped_energy)
{
  const int num_hits = ordered_hits.size();
  if (num_hits < 3) {
    return false;
  }

  double sum_escaped_energy = 0.0;
  for(int i_hit = 2; i_hit < num_hits; ++i_hit){
    const vector3_t incident_direction_at_lastsecondpoint = ordered_hits[i_hit - 1]->Position() - ordered_hits[i_hit - 2]->Position();
    const vector3_t scattering_direction_at_lastsecondpoint = ordered_hits[i_hit]->Position() - ordered_hits[i_hit - 1]->Position();

    const double cos_theta_geom = cosThetaGeometry(incident_direction_at_lastsecondpoint, scattering_direction_at_lastsecondpoint);
    const double scattered_energy_at_lasesecondpoint = scatteredGammarayEnergyFromEdep(ordered_hits[i_hit - 1]->Energy(), cos_theta_geom);
    
    double escaped_energy = scattered_energy_at_lasesecondpoint;
    for(int j_hit = i_hit; j_hit < num_hits; ++j_hit){
        escaped_energy -= ordered_hits[j_hit]->Energy();
    }
    sum_escaped_energy += escaped_energy;
  }

  escaped_energy = sum_escaped_energy/(num_hits - 2);
  return true;
}

} /* namespace compton_constraints */
} /* namespace comptonsoft */
