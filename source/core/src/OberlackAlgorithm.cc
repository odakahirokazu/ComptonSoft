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

#include "OberlackAlgorithm.hh"
#include <algorithm>
#include <cmath>
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "ComptonConstraints.hh"
#include <boost/property_tree/json_parser.hpp>

namespace unit = anlgeant4::unit;

namespace comptonsoft {

using namespace compton_constraints;

OberlackAlgorithm::OberlackAlgorithm()
  : sigma_level_energy_margin_for_checkScatteringAngle_(5.0),
    process_mode(2), // 0: assume full deposit & escapoe simul, 1: first assume full dep, if no order is OK, then assume escape, 2: only full deposit, 3: only escape
    assume_initial_gammaray_energy(false),
    known_initial_gammaray_energy(1000.0 * unit::keV),
    assume_initial_direction(false),
    use_averaged_escaped_energy(true),
    par0_energy_resolution_(5.0),
    par1_energy_resolution_(0.5),
    par2_energy_resolution_(0.0),
    consider_position_resolution(true),
    position_resolution_x_(0.2 * unit::cm),
    position_resolution_y_(0.2 * unit::cm),
    position_resolution_z_(0.2 * unit::cm),
    escape_weight_(1.0),
    cross_section_filename_("crosssection.root")
{
  setParameterFile("parfile_Oberlack.json");
}

OberlackAlgorithm::~OberlackAlgorithm() = default;

bool OberlackAlgorithm::loadParameters(boost::property_tree::ptree& pt)
{
  sigma_level_energy_margin_for_checkScatteringAngle_ = pt.get<double>("Oberlack.sigma_level_energy_margin_for_checkScatteringAngle");
  process_mode = pt.get<int>("Oberlack.process_mode");
  assume_initial_gammaray_energy = pt.get<bool>("Oberlack.assume_initial_gammaray_energy");
  known_initial_gammaray_energy = pt.get<double>("Oberlack.known_initial_gammaray_energy") * unit::keV;
  use_averaged_escaped_energy = pt.get<bool>("Oberlack.use_averaged_escaped_energy");
  par0_energy_resolution_ = pt.get<double>("Oberlack.energy_resolution.par0");
  par1_energy_resolution_ = pt.get<double>("Oberlack.energy_resolution.par1");
  par2_energy_resolution_ = pt.get<double>("Oberlack.energy_resolution.par2");
  consider_position_resolution = pt.get<bool>("Oberlack.consider_position_resolution");
  position_resolution_x_ = pt.get<double>("Oberlack.position_resolution.x") * unit::cm;
  position_resolution_y_ = pt.get<double>("Oberlack.position_resolution.y") * unit::cm;
  position_resolution_z_ = pt.get<double>("Oberlack.position_resolution.z") * unit::cm;
    
  try{
    assume_initial_direction = pt.get<bool>("Oberlack.assume_initial_direction");
    if(assume_initial_direction){
      known_initial_direction.set( pt.get<double>("Oberlack.known_initial_direction.x"),
                                   pt.get<double>("Oberlack.known_initial_direction.y"),
                                   pt.get<double>("Oberlack.known_initial_direction.z") );
    }
  }catch (...){

  }

  escape_weight_ = pt.get<double>("Oberlack.escape_weight");
  cross_section_filename_ = pt.get<std::string>("Oberlack.cross_section_filename");
  
  cross_section_file_ = new TFile(cross_section_filename_.c_str(), "r");
  tg_cross_section_compton_ = (TGraph*)cross_section_file_->Get("compton");
  tg_cross_section_phot_abs_ = (TGraph*)cross_section_file_->Get("phot_abs");
  tg_cross_section_pair_ = (TGraph*)cross_section_file_->Get("pair");
  tg_cross_section_tot_ = (TGraph*)cross_section_file_->Get("tot_wo_coherent");

  std::cout << std::endl;
  std::cout << "--- Oberlack ---" << std::endl;
  std::cout << "sigma_level_energy_margin_for_checkScatteringAngle_ : " << sigma_level_energy_margin_for_checkScatteringAngle_ << std::endl;
  std::cout << "process_mode : " << process_mode << std::endl;
  std::cout << "assume_initial_gammaray_energy : " << assume_initial_gammaray_energy << std::endl;
  if (assume_initial_gammaray_energy) {
    std::cout << "known_initial_gammaray_energy (keV) : " << known_initial_gammaray_energy / unit::keV << std::endl;
  }
  std::cout << "use_averaged_escaped_energy : " << use_averaged_escaped_energy << std::endl;
  std::cout << "par0_energy_resolution_ : " << par0_energy_resolution_ << std::endl;
  std::cout << "par1_energy_resolution_ : " << par1_energy_resolution_ << std::endl;
  std::cout << "par2_energy_resolution_ : " << par2_energy_resolution_ << std::endl;
  std::cout << "cross_section_filename_ : " << cross_section_filename_ << std::endl;
  std::cout << "consider_position_resolution : " << consider_position_resolution << std::endl;
  if (consider_position_resolution) {
    std::cout << "position_resolution_x_ (cm) : " << position_resolution_x_ / unit::cm << std::endl;
    std::cout << "position_resolution_y_ (cm) : " << position_resolution_y_ / unit::cm << std::endl;
    std::cout << "position_resolution_z_ (cm) : " << position_resolution_z_ / unit::cm << std::endl;
  }
  if (assume_initial_direction) {
    std::cout << "assume_initial_direction : " << assume_initial_direction << std::endl;
    std::cout << "known_initial_direction (x) : " << known_initial_direction.x() << std::endl;
    std::cout << "known_initial_direction (y) : " << known_initial_direction.y() << std::endl;
    std::cout << "known_initial_direction (z) : " << known_initial_direction.z() << std::endl;
  }
  std::cout << "escape_weight: " << escape_weight_ << std::endl;

  if(Kroger_mode){
    std::cout << "!!!WARNING!!! Kroger_mode is set to TRUE !!!" << std::endl;
  }

  return true;
}

void OberlackAlgorithm::initializeEvent()
{
}

bool OberlackAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  setTotalEnergyDepositsAndNumHits(hits);

  if (num_hits_ > MaxHits() || num_hits_< 2) {
    return false;
  }

  bool result = false;
  if (process_mode == 0 || process_mode == 1 || process_mode == 2) {
    result = reconstructFullDepositEvent(hits, baseEvent, eventsReconstructed);
  }

  if (process_mode == 0 || (process_mode == 1 && !result) || process_mode == 3) {
    result |= reconstructEscapeEvent(hits, baseEvent, eventsReconstructed);
  }

  if (result == true) {
    normalizeReconstructionFraction(eventsReconstructed);
    if (selecting_maximum_likelihood_order_) {
      selectMaximumLikelihoodOrder(eventsReconstructed);
    }
    return true;
  }
  return false;
}

bool OberlackAlgorithm::
reconstructFullDepositEvent(const std::vector<DetectorHit_sptr>& hits,
                            const BasicComptonEvent& baseEvent,
                            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  bool result = false;

  std::vector<int> scattering_order(num_hits_);
  for (int i_hit = 0; i_hit < num_hits_; ++i_hit){
    scattering_order[i_hit] = i_hit;
  }

  int reconstructedOrder = 0;
  do {
    auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);

    std::vector<DetectorHit_sptr> ordered_hits(num_hits_);
    for (int i_hit = 0; i_hit < num_hits_; ++i_hit){
      ordered_hits[i_hit] = hits[ scattering_order[i_hit] ];
    }

    bool this_result;
    if (assume_initial_gammaray_energy){
      this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, known_initial_gammaray_energy);
    }
    if (Kroger_mode){
      double escaped_energy = 0.0;
      if (use_averaged_escaped_energy) {
        this_result = estimateAveragedEscapedEnergy(ordered_hits, escaped_energy);
      } else {
        this_result = estimateEscapedEnergy(ordered_hits, escaped_energy);
      }
      if (!this_result) { continue; }
      const double corrected_total_energy = total_energy_deposits_ + escaped_energy;
      this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, corrected_total_energy);
    }
    else{
      this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, total_energy_deposits_);
    }

    if (this_result) {
      eventReconstructed->setReconstructedOrder(reconstructedOrder);
      eventsReconstructed.push_back(eventReconstructed);
      result = this_result;
    }
    
    ++reconstructedOrder;
  } while (std::next_permutation(scattering_order.begin(), scattering_order.end()));

  return result;
}

bool OberlackAlgorithm::
reconstructEscapeEvent(const std::vector<DetectorHit_sptr>& hits,
                       const BasicComptonEvent& baseEvent,
                       std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  bool result = false;

  std::vector<int> scattering_order(num_hits_);
  for (int i_hit = 0; i_hit < num_hits_; ++i_hit) {
    scattering_order[i_hit] = i_hit;
  }

  int reconstructedOrder = 0;
  do {
    auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);

    std::vector<DetectorHit_sptr> ordered_hits(num_hits_);
    for (int i_hit = 0; i_hit < num_hits_; ++i_hit) {
      ordered_hits[i_hit] = hits[ scattering_order[i_hit] ];
    }
    
    bool this_result;
    if (assume_initial_gammaray_energy){
      this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, known_initial_gammaray_energy, true);
    }
    else{
      double escaped_energy = 0.0;
      if (use_averaged_escaped_energy) {
        this_result = estimateAveragedEscapedEnergy(ordered_hits, escaped_energy);
      } else {
        this_result = estimateEscapedEnergy(ordered_hits, escaped_energy);
      }
      if (!this_result) { continue; }
      const double corrected_total_energy = total_energy_deposits_ + escaped_energy;
      this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, corrected_total_energy, true);
    }
   
    if (this_result) {
      eventReconstructed->setReconstructedOrder(reconstructedOrder);
      eventsReconstructed.push_back(eventReconstructed);
      result = this_result;
    }

    ++reconstructedOrder;
  } while (std::next_permutation(scattering_order.begin(), scattering_order.end()));

  return result;
}

bool OberlackAlgorithm::
reconstructOrderedHits(const std::vector<DetectorHit_sptr>& ordered_hits,
                       BasicComptonEvent& eventReconstructed,
                       double incident_energy,
                       bool is_escape_event)
{
  for (int i_hit = 1; i_hit <= num_hits_ - 2; ++i_hit) {
    double gammaray_energy_before_ihit = incident_energy;
    for (int j_hit = 0; j_hit <= i_hit - 1; ++j_hit) {
      gammaray_energy_before_ihit -= ordered_hits[j_hit]->Energy();
    }
    double energy_margin_for_checkScatteringAngle_ = sigma_level_energy_margin_for_checkScatteringAngle_ * getEnergyResolution(ordered_hits[i_hit]->Energy());
    const bool is_this_scattering_acceptable = checkScatteringAngle(ordered_hits[i_hit - 1], 
                                                                    ordered_hits[i_hit],
                                                                    ordered_hits[i_hit + 1],
                                                                    gammaray_energy_before_ihit,
                                                                    energy_margin_for_checkScatteringAngle_);
    if (!is_this_scattering_acceptable) { return false; }
  }

  double FOM_ = calFOM(ordered_hits, incident_energy, is_escape_event);
  if (FOM_ == 0.0) { return false; }

  eventReconstructed.setHit1(0, ordered_hits[0]);
  eventReconstructed.setHit2(1, ordered_hits[1]);
  eventReconstructed.setHit2Energy(incident_energy - ordered_hits[0]->Energy());
  eventReconstructed.setNumberOfHits(num_hits_);
  eventReconstructed.setEscapeFlag(is_escape_event);
  eventReconstructed.setTotalEnergyDeposit(total_energy_deposits_);
  eventReconstructed.setLikelihood(FOM_);

  const double this_reconstruction_fraction = FOM_;
  eventReconstructed.setReconstructionFraction(this_reconstruction_fraction);

  return true;
}

double OberlackAlgorithm::
calFOM(const std::vector<DetectorHit_sptr>& ordered_hits,
       double incident_energy,
       bool is_escape_event) 
{
  double FOM_ = 1.0;

  for (int i_hit = 0; i_hit < num_hits_; ++i_hit) {
    if (i_hit == 0) {
      double cos_theta_first_scattering = cosThetaKinematics( incident_energy, 
                                                              ordered_hits[0]->Energy() ); 
      if (cos_theta_first_scattering < -1.0 || 1.0 < cos_theta_first_scattering) {
        FOM_ = 0.0;
        break;
      }
      
      if(assume_initial_direction){
        FOM_ *= compScatteringAngleWithInitialDirection(ordered_hits, incident_energy, is_escape_event); 
      }
    }
    else if (i_hit == num_hits_ - 1) {

    }
    else {
      FOM_ *= compScatteringAngle(ordered_hits, i_hit, incident_energy, is_escape_event); 
    }
  }
  
  if (is_escape_event) { FOM_ *= escape_weight_; }

  return FOM_;
}

void OberlackAlgorithm::setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits)
{
  total_energy_deposits_ = total_energy_deposits(hits);
  num_hits_ = hits.size();
}

void OberlackAlgorithm::normalizeReconstructionFraction(std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  double sum_reconstruction_fraction = 0.0;
  for (std::size_t i = 0; i < eventsReconstructed.size(); ++i) {
    sum_reconstruction_fraction += eventsReconstructed[i]->ReconstructionFraction();
  }
  for (std::size_t i = 0; i < eventsReconstructed.size(); ++i) {
    eventsReconstructed[i]->setReconstructionFraction(eventsReconstructed[i]->ReconstructionFraction() / sum_reconstruction_fraction);
  }
}

void OberlackAlgorithm::selectMaximumLikelihoodOrder(std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  using event_t = BasicComptonEvent_sptr;
  const event_t event_yielding_maximum_likelihood =
    *std::max_element(eventsReconstructed.begin(),
                      eventsReconstructed.end(),
                      [](const event_t& a, const event_t& b) {
                        return a->Likelihood() < b->Likelihood();
                      });
  eventsReconstructed.clear();
  eventsReconstructed.push_back(event_yielding_maximum_likelihood);
  if( std::isnan(event_yielding_maximum_likelihood->ReconstructionFraction()) ){
    std::cout << "------ WARNING!!! ------" << std::endl;
    std::cout << "ReconstructionFraction is NaN." << std::endl;
    std::cout << "------------------------" << std::endl;
  }
}

double OberlackAlgorithm::getEnergyResolution(double energy)
{
  return std::sqrt( std::pow(par0_energy_resolution_, 2)
                    + std::pow(par1_energy_resolution_, 2) * energy / unit::keV
                    + std::pow(par2_energy_resolution_, 2) * std::pow(energy / unit::keV, 2) ) * unit::keV; //MeV
}

double OberlackAlgorithm::getErrorCosThetaGeom(const vector3_t& incident_direction, const vector3_t& scattering_direction)
{
  const double cos_theta_ = cosThetaGeometry(incident_direction, scattering_direction);
  
  const double dcos_dx_A = scattering_direction.x() / scattering_direction.mag() / incident_direction.mag()
                         - incident_direction.x() / incident_direction.mag2() * cos_theta_;
  const double dcos_dx_B = incident_direction.x() / scattering_direction.mag() / incident_direction.mag()
                         - scattering_direction.x() / scattering_direction.mag2() * cos_theta_;
  const double dcos_dx_sq = 2 * ( std::pow(dcos_dx_A, 2) + std::pow(dcos_dx_B, 2) - dcos_dx_A * dcos_dx_B );
  
  const double dcos_dy_A = scattering_direction.y() / scattering_direction.mag() / incident_direction.mag()
                         - incident_direction.y() / incident_direction.mag2() * cos_theta_;
  const double dcos_dy_B = incident_direction.y() / scattering_direction.mag() / incident_direction.mag()
                         - scattering_direction.y() / scattering_direction.mag2() * cos_theta_;
  const double dcos_dy_sq = 2 * ( std::pow(dcos_dy_A, 2) + std::pow(dcos_dy_B, 2) - dcos_dy_A * dcos_dy_B );

  const double dcos_dz_A = scattering_direction.z() / scattering_direction.mag() / incident_direction.mag()
                         - incident_direction.z() / incident_direction.mag2() * cos_theta_;
  const double dcos_dz_B = incident_direction.z() / scattering_direction.mag() / incident_direction.mag()
                         - scattering_direction.z() / scattering_direction.mag2() * cos_theta_;
  const double dcos_dz_sq = 2 * ( std::pow(dcos_dz_A, 2) + std::pow(dcos_dz_B, 2) - dcos_dz_A * dcos_dz_B );

  const double error2 = dcos_dx_sq * std::pow(position_resolution_x_, 2) 
                      + dcos_dy_sq * std::pow(position_resolution_y_, 2) 
                      + dcos_dz_sq * std::pow(position_resolution_z_, 2);

  return std::sqrt(error2);
}

double OberlackAlgorithm::
compScatteringAngle(const std::vector<DetectorHit_sptr>& ordered_hits, int i_hit, double incident_energy, bool /* is_escape_event */)
{
    double gammaray_energy_before_ihit = incident_energy;
    for(int j_hit = 0; j_hit < i_hit; ++j_hit){
      gammaray_energy_before_ihit -= ordered_hits[j_hit]->Energy(); 
    }
    double gammaray_energy_after_ihit = gammaray_energy_before_ihit - ordered_hits[i_hit]->Energy(); 

    const double cos_theta_geom = cosThetaGeometry(ordered_hits[i_hit]->Position() - ordered_hits[i_hit - 1]->Position(),
                                                   ordered_hits[i_hit + 1]->Position() - ordered_hits[i_hit]->Position());
    const double cos_theta_kin = cosThetaKinematics( gammaray_energy_before_ihit,
                                                     ordered_hits[i_hit]->Energy() ); 
    
    const double delta_e_dep = getEnergyResolution(ordered_hits[i_hit]->Energy());
    double delta_E_scatteredgamma = 0;
    for(int j_hit = i_hit + 1; j_hit < (int)ordered_hits.size(); ++j_hit){
        delta_E_scatteredgamma += std::pow(getEnergyResolution(ordered_hits[j_hit]->Energy()), 2);
    }
    delta_E_scatteredgamma = std::sqrt(delta_E_scatteredgamma);

    const double delta_cos_theta_kin = CLHEP::electron_mass_c2 
                                 * std::sqrt( std::pow(delta_e_dep, 2) / std::pow(gammaray_energy_before_ihit, 4)
                                            + std::pow(delta_E_scatteredgamma, 2) * std::pow( 1.0 / std::pow(gammaray_energy_after_ihit, 2) - 1.0 / std::pow(gammaray_energy_before_ihit, 2), 2) );
    
    double sigma;
    if(consider_position_resolution){
      const double delta_cos_theta_geom = getErrorCosThetaGeom(ordered_hits[i_hit]->Position() - ordered_hits[i_hit - 1]->Position(),
                                                              ordered_hits[i_hit + 1]->Position() - ordered_hits[i_hit]->Position());
      sigma = std::sqrt( std::pow(delta_cos_theta_kin, 2) + std::pow(delta_cos_theta_geom, 2) );       
    }else{
      sigma = delta_cos_theta_kin;
    }

    double prob = 0.0;
    prob = std::exp( - std::pow( (cos_theta_kin - cos_theta_geom) / sigma, 2) );
    
    return prob;
}

double OberlackAlgorithm::
compScatteringAngleWithInitialDirection(const std::vector<DetectorHit_sptr>& ordered_hits, double incident_energy, bool /* is_escape_event */)
{
    double gammaray_energy_before_ihit = incident_energy;
    double gammaray_energy_after_ihit = gammaray_energy_before_ihit - ordered_hits[0]->Energy(); 

    double cos_theta_geom = cosThetaGeometry( known_initial_direction,
                                                    ordered_hits[1]->Position() - ordered_hits[0]->Position() );
    double cos_theta_kin = cosThetaKinematics( gammaray_energy_before_ihit,
                                                     ordered_hits[0]->Energy() ); 

    const double delta_e_dep = getEnergyResolution(ordered_hits[0]->Energy());
    double delta_E_scatteredgamma = 0;
    for(int j_hit = 1; j_hit < (int)ordered_hits.size(); ++j_hit){
        delta_E_scatteredgamma += std::pow(getEnergyResolution(ordered_hits[j_hit]->Energy()), 2);
    }
    delta_E_scatteredgamma = std::sqrt(delta_E_scatteredgamma);

    const double delta_cos_theta_kin = CLHEP::electron_mass_c2 
                                 * std::sqrt( std::pow(delta_e_dep, 2) / std::pow(gammaray_energy_before_ihit, 4)
                                            + std::pow(delta_E_scatteredgamma, 2) * std::pow( 1.0 / std::pow(gammaray_energy_after_ihit, 2) - 1.0 / std::pow(gammaray_energy_before_ihit, 2), 2) );
    
    double sigma = delta_cos_theta_kin;

    double prob = std::exp( - std::pow( (cos_theta_kin - cos_theta_geom) / sigma, 2) );

    return prob;
}

} /* namespace comptonsoft */
