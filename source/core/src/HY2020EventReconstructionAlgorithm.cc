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

#include "HY2020EventReconstructionAlgorithm.hh"
#include <algorithm>
#include <cmath>
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "ComptonConstraints.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace unit = anlgeant4::unit;

namespace comptonsoft {

using namespace compton_constraints;

HY2020EventReconstructionAlgorithm::HY2020EventReconstructionAlgorithm()
  : sigma_level_energy_margin_for_checkScatteringAngle_(5.0),
    process_mode(0), // 0: assume full deposit & escapoe simul, 1: first assume full dep, if no order is OK, then assume escape, 2: only full deposit, 3: only escape
    assume_initial_gammaray_energy(false),
    known_initial_gammaray_energy(1000.0 * unit::keV),
    use_averaged_escaped_energy(true),
    detector_length_scale(15.0), //cm
    par0_energy_resolution_(5.0), 
    par1_energy_resolution_(0.5),
    par2_energy_resolution_(0.0),
    cross_section_filename_("/Users/yoneda/Work/Exp/GRAMS/reconstruction/cross_section_data/argon/crosssection_argron.root")
{
    if (FILE *file = fopen(parfilename.c_str(), "r")) {
        fclose(file);
        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(parfilename.c_str(), pt);

        sigma_level_energy_margin_for_checkScatteringAngle_ = pt.get<double>("HY2020.sigma_level_energy_margin_for_checkScatteringAngle");
        process_mode = pt.get<int>("HY2020.process_mode");
        assume_initial_gammaray_energy = pt.get<bool>("HY2020.assume_initial_gammaray_energy");
        known_initial_gammaray_energy = pt.get<double>("HY2020.known_initial_gammaray_energy") * unit::keV;
        use_averaged_escaped_energy = pt.get<bool>("HY2020.use_averaged_escaped_energy");
        detector_length_scale = pt.get<double>("HY2020.detector_length_scale");
        par0_energy_resolution_ = pt.get<double>("HY2020.energy_resolution.par0");
        par1_energy_resolution_ = pt.get<double>("HY2020.energy_resolution.par1");
        par2_energy_resolution_ = pt.get<double>("HY2020.energy_resolution.par2");
        cross_section_filename_ = pt.get<std::string>("HY2020.cross_section_filename");
    }

    cross_section_file_ = new TFile(cross_section_filename_.c_str(), "r");
    tg_cross_section_compton_ = (TGraph*)cross_section_file_->Get("compton");
    tg_cross_section_phot_abs_ = (TGraph*)cross_section_file_->Get("phot_abs");
    tg_cross_section_pair_ = (TGraph*)cross_section_file_->Get("pair");
    tg_cross_section_tot_ = (TGraph*)cross_section_file_->Get("tot_wo_coherent");

    std::cout << std::endl;
    std::cout << "--- HY2020 ---" << std::endl;
    std::cout << "sigma_level_energy_margin_for_checkScatteringAngle_ : " << sigma_level_energy_margin_for_checkScatteringAngle_ << std::endl;
    std::cout << "process_mode : " << process_mode << std::endl;
    std::cout << "assume_initial_gammaray_energy : " << assume_initial_gammaray_energy << std::endl;
    std::cout << "known_initial_gammaray_energy (keV) : " << known_initial_gammaray_energy << std::endl;
    std::cout << "use_averaged_escaped_energy : " << use_averaged_escaped_energy << std::endl;
    std::cout << "detector_length_scale (cm) : " << detector_length_scale << std::endl;
    std::cout << "par0_energy_resolution_ : " << par0_energy_resolution_ << std::endl;
    std::cout << "par1_energy_resolution_ : " << par1_energy_resolution_ << std::endl;
    std::cout << "par2_energy_resolution_ : " << par2_energy_resolution_ << std::endl;
    std::cout << "cross_section_filename_ : " << cross_section_filename_ << std::endl;
}

HY2020EventReconstructionAlgorithm::~HY2020EventReconstructionAlgorithm() = default;

void HY2020EventReconstructionAlgorithm::initializeEvent()
{
}

bool HY2020EventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  setTotalEnergyDepositsAndNumHits(hits);

  if (num_hits_ > MaxHits() || num_hits_< 2) {
    return false;
  }

  bool result = false;
  if(process_mode == 0 || process_mode == 1 || process_mode == 2){
    result = reconstructFullDepositEvent(hits, baseEvent, eventsReconstructed);
  }

  if(process_mode == 0 || (process_mode == 1 && !result) || process_mode == 3){
    result |= reconstructEscapeEvent(hits, baseEvent, eventsReconstructed);
  }

  if(result == true){
    normalizeReconstructionFraction(eventsReconstructed);
    return true;
  }
  return false;
}

bool HY2020EventReconstructionAlgorithm::
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
    for(int i_hit = 0; i_hit < num_hits_; ++i_hit){
      ordered_hits[i_hit] = hits[ scattering_order[i_hit] ];
    }

    bool this_result;
    if(assume_initial_gammaray_energy){
      this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, known_initial_gammaray_energy);
    }else{
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

bool HY2020EventReconstructionAlgorithm::
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
    if(assume_initial_gammaray_energy){
      this_result = reconstructOrderedHits(ordered_hits, *eventReconstructed, known_initial_gammaray_energy, true);
    }else{
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

bool HY2020EventReconstructionAlgorithm::
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

  double likelihood_ = calLikelihood(ordered_hits, incident_energy, is_escape_event);
  if(likelihood_ == 0.0) { return false; }

  eventReconstructed.setHit1(0, ordered_hits[0]);
  eventReconstructed.setHit2(1, ordered_hits[1]);
  eventReconstructed.setHit2Energy(incident_energy - ordered_hits[0]->Energy());
  eventReconstructed.setNumberOfHits(num_hits_);
  eventReconstructed.setEscapeFlag(is_escape_event);
  eventReconstructed.setTotalEnergyDeposit(total_energy_deposits_);
  eventReconstructed.setLikelihood(likelihood_);

  const double this_reconstruction_fraction = likelihood_;
  eventReconstructed.setReconstructionFraction(this_reconstruction_fraction);

  return true;
}

double HY2020EventReconstructionAlgorithm::
calLikelihood(const std::vector<DetectorHit_sptr>& ordered_hits,
              double incident_energy,
              bool is_escape_event) 
{
  double likelihood_ = 1.0;

  double gammaray_energy_before_ihit = incident_energy;
  for (int i_hit = 0; i_hit < num_hits_; ++i_hit) {
    if (i_hit == 0) {
      double cos_theta_first_scattering = cosThetaKinematics( incident_energy, 
                                                              ordered_hits[0]->Energy() ); 
      if(cos_theta_first_scattering < -1.0 || 1.0 < cos_theta_first_scattering) {
        likelihood_ = 0.0;
        break;
      }

      likelihood_ *= probComptonFirst(gammaray_energy_before_ihit);
      likelihood_ *= normalized_differentialCrossSection( incident_energy, cos_theta_first_scattering );
      likelihood_ *= probEnergyDetection( ordered_hits[0]->Energy(), ordered_hits[0]->Energy() ); 

      gammaray_energy_before_ihit -= ordered_hits[0]->Energy();
    } else if (i_hit == num_hits_ - 1) {
      double path_length = (ordered_hits[i_hit]->Position() - ordered_hits[i_hit - 1]->Position()).mag() / unit::cm;
      if( is_escape_event ) {
        likelihood_ *= probCompton(gammaray_energy_before_ihit, path_length);
        likelihood_ *= probEscape(gammaray_energy_before_ihit, ordered_hits[i_hit]->Energy());
      } else {
        likelihood_ *= probAbsorption(gammaray_energy_before_ihit, path_length);
        likelihood_ *= probEnergyDetection( gammaray_energy_before_ihit, ordered_hits[i_hit]->Energy() ); 
      }
    } else {
      double path_length = (ordered_hits[i_hit]->Position() - ordered_hits[i_hit - 1]->Position()).mag() / unit::cm;
      double cos_theta_geom = cosThetaGeometry( ordered_hits[i_hit]->Position() - ordered_hits[i_hit - 1]->Position(),
                                                ordered_hits[i_hit + 1]->Position() - ordered_hits[i_hit]->Position() );
      double energy_deposit = energyDeposit( gammaray_energy_before_ihit, cos_theta_geom );

      likelihood_ *= probCompton(gammaray_energy_before_ihit, path_length);
      likelihood_ *= normalized_differentialCrossSection( gammaray_energy_before_ihit, cos_theta_geom );
      likelihood_ *= probEnergyDetection( energy_deposit, ordered_hits[i_hit]->Energy() ); 

      gammaray_energy_before_ihit -= energy_deposit;
    }
  }
  return likelihood_;
}

void HY2020EventReconstructionAlgorithm::setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits)
{
  total_energy_deposits_ = total_energy_deposits(hits);
  num_hits_ = hits.size();
}

void HY2020EventReconstructionAlgorithm::normalizeReconstructionFraction(std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  double sum_reconstruction_fraction = 0.0;
  for (std::size_t i = 0; i < eventsReconstructed.size(); ++i) {
    sum_reconstruction_fraction += eventsReconstructed[i]->ReconstructionFraction();
  }
  for(std::size_t i = 0; i < eventsReconstructed.size(); ++i) {
    eventsReconstructed[i]->setReconstructionFraction(eventsReconstructed[i]->ReconstructionFraction() / sum_reconstruction_fraction);
  }
}

double HY2020EventReconstructionAlgorithm::getEnergyResolution(double energy){
    return std::sqrt( std::pow(par0_energy_resolution_, 2)
                    + std::pow(par1_energy_resolution_, 2) * energy / unit::keV
                    + std::pow(par2_energy_resolution_, 2) * std::pow(energy / unit::keV, 2) ) * unit::keV; //MeV
}

double HY2020EventReconstructionAlgorithm::
probEnergyDetection(double energy_real, double energy_detected)
{
    const double sigma_energy_2 = std::pow( getEnergyResolution(energy_real), 2);
    double prob = 1.0 / std::sqrt( 2 * CLHEP::pi * sigma_energy_2)
                * std::exp( - std::pow(energy_real - energy_detected, 2) / 2.0 / sigma_energy_2 );
    
    return prob;
}

double HY2020EventReconstructionAlgorithm::
probCompton(double gammaray_energy_before_hit, double path_length)
{
    const double cs_tot_ = tg_cross_section_tot_->Eval(gammaray_energy_before_hit);
    const double cs_compton_ = tg_cross_section_compton_->Eval(gammaray_energy_before_hit);
    double prob = std::exp( -1 * cs_tot_ * path_length ) * cs_compton_ / std::pow( path_length, 2 );

    return prob;
}

double HY2020EventReconstructionAlgorithm::
probComptonFirst(double gammaray_energy_before_hit) {
    const double cs_tot_ = tg_cross_section_tot_->Eval(gammaray_energy_before_hit);
    const double cs_compton_ = tg_cross_section_compton_->Eval(gammaray_energy_before_hit);
    double prob = std::exp( -1 * cs_tot_ * detector_length_scale) * cs_compton_;

    return prob;
}

double HY2020EventReconstructionAlgorithm::
probAbsorption(double gammaray_energy_before_hit, double path_length)
{
    const double cs_tot_ = tg_cross_section_tot_->Eval(gammaray_energy_before_hit);
    const double cs_phot_abs_ = tg_cross_section_phot_abs_->Eval(gammaray_energy_before_hit);
    double prob = std::exp( -1 * cs_tot_ * path_length ) * cs_phot_abs_ / std::pow( path_length, 2 );

    return prob;
}

double HY2020EventReconstructionAlgorithm::
probEscape(double gammaray_energy_before_lasthit, double energy_deposit_lasthit)
{
    const double cs_tot_escape = tg_cross_section_tot_->Eval( gammaray_energy_before_lasthit - energy_deposit_lasthit );
    const double cos_theta_last_scattering = cosThetaKinematics( gammaray_energy_before_lasthit, energy_deposit_lasthit);
    if(cos_theta_last_scattering < -1.0 || 1.0 < cos_theta_last_scattering) {
        return 0.0;
    }
    
    double prob = 2 * CLHEP::pi * normalized_differentialCrossSection( gammaray_energy_before_lasthit, cos_theta_last_scattering );
    prob *= std::exp( -1 * cs_tot_escape * detector_length_scale );
    prob *= CLHEP::electron_mass_c2 / pow(gammaray_energy_before_lasthit - energy_deposit_lasthit, 2);

    return prob;
}

} /* namespace comptonsoft */
