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

#include "ComptonEventTreeIO.hh"
#include "AstroUnits.hh"
#include "TTree.h"
#include "BasicComptonEvent.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

ComptonEventTreeIO::ComptonEventTreeIO()
  : cetree_(nullptr)
{
}

ComptonEventTreeIO::~ComptonEventTreeIO() = default;

void ComptonEventTreeIO::defineBranches()
{
  cetree_->Branch("eventid", &eventid_, "eventid/l");
  cetree_->Branch("num_reconstruction_cases", &num_reconstruction_cases_, "num_reconstruction_cases/S");
  cetree_->Branch("num_hits", &num_hits_, "num_hits/S");

  cetree_->Branch("hit1_id", &hit1_id_, "hit1_id/S");
  cetree_->Branch("hit1_process", &hit1_process_, "hit1_process/i");
  cetree_->Branch("hit1_detector", &hit1_detector_, "hit1_detector/S");
  cetree_->Branch("hit1_readout_module", &hit1_readout_module_, "hit1_readout_module/S");
  cetree_->Branch("hit1_section", &hit1_section_, "hit1_section/S");
  cetree_->Branch("hit1_channel", &hit1_channel_, "hit1_channel/S");
  cetree_->Branch("hit1_pixelx", &hit1_pixelx_, "hit1_pixelx/S");
  cetree_->Branch("hit1_pixely", &hit1_pixely_, "hit1_pixely/S");
  cetree_->Branch("hit1_pixelz", &hit1_pixelz_, "hit1_pixelz/S");

  cetree_->Branch("hit1_time", &hit1_time_, "hit1_time/F");
  cetree_->Branch("hit1_posx", &hit1_posx_, "hit1_posx/F");
  cetree_->Branch("hit1_posy", &hit1_posy_, "hit1_posy/F");
  cetree_->Branch("hit1_posz", &hit1_posz_, "hit1_posz/F");
  cetree_->Branch("hit1_energy", &hit1_energy_, "hit1_energy/F");
  cetree_->Branch("hit1_time_error", &hit1_time_error_, "hit1_time_error/F");
  cetree_->Branch("hit1_posx_error", &hit1_posx_error_, "hit1_posx_error/F");
  cetree_->Branch("hit1_posy_error", &hit1_posy_error_, "hit1_posy_error/F");
  cetree_->Branch("hit1_posz_error", &hit1_posz_error_, "hit1_posz_error/F");
  cetree_->Branch("hit1_energy_error", &hit1_energy_error_, "hit1_energy_error/F");
  
  cetree_->Branch("hit2_id", &hit2_id_, "hit2_id/S");
  cetree_->Branch("hit2_process", &hit2_process_, "hit2_process/i");
  cetree_->Branch("hit2_detector", &hit2_detector_, "hit2_detector/S");
  cetree_->Branch("hit2_readout_module", &hit2_readout_module_, "hit2_readout_module/S");
  cetree_->Branch("hit2_section", &hit2_section_, "hit2_section/S");
  cetree_->Branch("hit2_channel", &hit2_channel_, "hit2_channel/S");
  cetree_->Branch("hit2_pixelx", &hit2_pixelx_, "hit2_pixelx/S");
  cetree_->Branch("hit2_pixely", &hit2_pixely_, "hit2_pixely/S");
  cetree_->Branch("hit2_pixelz", &hit2_pixelz_, "hit2_pixelz/S");

  cetree_->Branch("hit2_time", &hit2_time_, "hit2_time/F");
  cetree_->Branch("hit2_posx", &hit2_posx_, "hit2_posx/F");
  cetree_->Branch("hit2_posy", &hit2_posy_, "hit2_posy/F");
  cetree_->Branch("hit2_posz", &hit2_posz_, "hit2_posz/F");
  cetree_->Branch("hit2_energy", &hit2_energy_, "hit2_energy/F");
  cetree_->Branch("hit2_time_error", &hit2_time_error_, "hit2_time_error/F");
  cetree_->Branch("hit2_posx_error", &hit2_posx_error_, "hit2_posx_error/F");
  cetree_->Branch("hit2_posy_error", &hit2_posy_error_, "hit2_posy_error/F");
  cetree_->Branch("hit2_posz_error", &hit2_posz_error_, "hit2_posz_error/F");
  cetree_->Branch("hit2_energy_error", &hit2_energy_error_, "hit2_energy_error/F");

  cetree_->Branch("energy_reconstructed", &energy_reconstructed_, "energy_reconstructed/F");

  cetree_->Branch("flags", &flags_, "flags/l");
  cetree_->Branch("costheta", &costheta_, "costheta/F");
  cetree_->Branch("dtheta", &dtheta_, "dtheta_/F");
  cetree_->Branch("phi", &phi_, "phi/F");

  cetree_->Branch("hitpattern", &hitpattern_, "hitpattern/l");
  cetree_->Branch("grade", &grade_, "grade/I");
  cetree_->Branch("likelihood", &likelihood_, "likelihood/D");

  cetree_->Branch("escape_flag", &escape_flag_, "escape_flag/O");
  cetree_->Branch("total_energy_deposit", &total_energy_deposit_, "total_energy_deposit/F");
  cetree_->Branch("reconstructed_order", &reconstructed_order_, "reconstructed_order/I");
  cetree_->Branch("reconstruction_fraction", &reconstruction_fraction_, "reconstruction_fraction/D");
}

void ComptonEventTreeIO::setBranchAddresses()
{
  cetree_->SetBranchAddress("eventid", &eventid_);
  cetree_->SetBranchAddress("num_reconstruction_cases", &num_reconstruction_cases_);
  cetree_->SetBranchAddress("num_hits", &num_hits_);

  cetree_->SetBranchAddress("hit1_id", &hit1_id_);
  cetree_->SetBranchAddress("hit1_process", &hit1_process_);
  cetree_->SetBranchAddress("hit1_detector", &hit1_detector_);
  cetree_->SetBranchAddress("hit1_readout_module", &hit1_readout_module_);
  cetree_->SetBranchAddress("hit1_section", &hit1_section_);
  cetree_->SetBranchAddress("hit1_channel", &hit1_channel_);
  cetree_->SetBranchAddress("hit1_pixelx", &hit1_pixelx_);
  cetree_->SetBranchAddress("hit1_pixely", &hit1_pixely_);
  cetree_->SetBranchAddress("hit1_pixelz", &hit1_pixelz_);

  cetree_->SetBranchAddress("hit1_time", &hit1_time_);
  cetree_->SetBranchAddress("hit1_posx", &hit1_posx_);
  cetree_->SetBranchAddress("hit1_posy", &hit1_posy_);
  cetree_->SetBranchAddress("hit1_posz", &hit1_posz_);
  cetree_->SetBranchAddress("hit1_energy", &hit1_energy_);
  cetree_->SetBranchAddress("hit1_time_error", &hit1_time_error_);
  cetree_->SetBranchAddress("hit1_posx_error", &hit1_posx_error_);
  cetree_->SetBranchAddress("hit1_posy_error", &hit1_posy_error_);
  cetree_->SetBranchAddress("hit1_posz_error", &hit1_posz_error_);
  cetree_->SetBranchAddress("hit1_energy_error", &hit1_energy_error_);
  
  cetree_->SetBranchAddress("hit2_id", &hit2_id_);
  cetree_->SetBranchAddress("hit2_process", &hit2_process_);
  cetree_->SetBranchAddress("hit2_detector", &hit2_detector_);
  cetree_->SetBranchAddress("hit2_readout_module", &hit2_readout_module_);
  cetree_->SetBranchAddress("hit2_section", &hit2_section_);
  cetree_->SetBranchAddress("hit2_channel", &hit2_channel_);
  cetree_->SetBranchAddress("hit2_pixelx", &hit2_pixelx_);
  cetree_->SetBranchAddress("hit2_pixely", &hit2_pixely_);
  cetree_->SetBranchAddress("hit2_pixelz", &hit2_pixelz_);

  cetree_->SetBranchAddress("hit2_time", &hit2_time_);
  cetree_->SetBranchAddress("hit2_posx", &hit2_posx_);
  cetree_->SetBranchAddress("hit2_posy", &hit2_posy_);
  cetree_->SetBranchAddress("hit2_posz", &hit2_posz_);
  cetree_->SetBranchAddress("hit2_energy", &hit2_energy_);
  cetree_->SetBranchAddress("hit2_time_error", &hit2_time_error_);
  cetree_->SetBranchAddress("hit2_posx_error", &hit2_posx_error_);
  cetree_->SetBranchAddress("hit2_posy_error", &hit2_posy_error_);
  cetree_->SetBranchAddress("hit2_posz_error", &hit2_posz_error_);
  cetree_->SetBranchAddress("hit2_energy_error", &hit2_energy_error_);

  cetree_->SetBranchAddress("energy_reconstructed", &energy_reconstructed_);

  cetree_->SetBranchAddress("flags", &flags_);
  cetree_->SetBranchAddress("costheta", &costheta_);
  cetree_->SetBranchAddress("dtheta", &dtheta_);
  cetree_->SetBranchAddress("phi", &phi_);

  cetree_->SetBranchAddress("hitpattern", &hitpattern_);
  cetree_->SetBranchAddress("grade", &grade_);
  cetree_->SetBranchAddress("likelihood", &likelihood_);

  cetree_->SetBranchAddress("escape_flag", &escape_flag_);
  cetree_->SetBranchAddress("total_energy_deposit", &total_energy_deposit_);
  cetree_->SetBranchAddress("reconstructed_order", &reconstructed_order_);
  cetree_->SetBranchAddress("reconstruction_fraction", &reconstruction_fraction_);
}

void ComptonEventTreeIO::fillEvent(const int64_t eventID,
                                   const int numCases,
                                   const BasicComptonEvent& event)
{
  eventid_ = (eventID >= 0) ? eventID : event.EventID();
  num_reconstruction_cases_ = numCases;
  num_hits_ = event.NumberOfHits();

  hit1_id_ = event.Hit1ID();
  hit1_process_ = event.Hit1Process();
  hit1_detector_ = event.Hit1DetectorID();
  const ReadoutBasedChannelID hit1ReadoutChannelID = event.Hit1ReadoutChannelID();
  hit1_readout_module_ = hit1ReadoutChannelID.ReadoutModule();
  hit1_section_ = hit1ReadoutChannelID.Section();
  hit1_channel_ = hit1ReadoutChannelID.Channel();
  const VoxelID hit1Voxel = event.Hit1Voxel();
  hit1_pixelx_ = hit1Voxel.X();
  hit1_pixely_ = hit1Voxel.Y();
  hit1_pixelz_ = hit1Voxel.Z();

  hit1_time_ = event.Hit1Time() / unit::second;
  const vector3_t hit1Position = event.Hit1Position();
  hit1_posx_ = hit1Position.x() / unit::cm;
  hit1_posy_ = hit1Position.y() / unit::cm;
  hit1_posz_ = hit1Position.z() / unit::cm;
  hit1_energy_ = event.Hit1Energy() / unit::keV;

  hit1_time_error_ = event.Hit1TimeError() / unit::second;
  const vector3_t hit1PositionError = event.Hit1PositionError();
  hit1_posx_error_ = hit1PositionError.x() / unit::cm;
  hit1_posy_error_ = hit1PositionError.y() / unit::cm;
  hit1_posz_error_ = hit1PositionError.z() / unit::cm;
  hit1_energy_error_ = event.Hit1EnergyError() / unit::keV;

  hit2_id_ = event.Hit2ID();
  hit2_process_ = event.Hit2Process();
  hit2_detector_ = event.Hit2DetectorID();
  const ReadoutBasedChannelID hit2ReadoutChannelID = event.Hit2ReadoutChannelID();
  hit2_readout_module_ = hit2ReadoutChannelID.ReadoutModule();
  hit2_section_ = hit2ReadoutChannelID.Section();
  hit2_channel_ = hit2ReadoutChannelID.Channel();
  const VoxelID hit2Voxel = event.Hit2Voxel();
  hit2_pixelx_ = hit2Voxel.X();
  hit2_pixely_ = hit2Voxel.Y();
  hit2_pixelz_ = hit2Voxel.Z();

  hit2_time_ = event.Hit2Time() / unit::second;
  const vector3_t hit2Position = event.Hit2Position();
  hit2_posx_ = hit2Position.x() / unit::cm;
  hit2_posy_ = hit2Position.y() / unit::cm;
  hit2_posz_ = hit2Position.z() / unit::cm;
  hit2_energy_ = event.Hit2Energy() / unit::keV;

  hit2_time_error_ = event.Hit2TimeError() / unit::second;
  const vector3_t hit2PositionError = event.Hit2PositionError();
  hit2_posx_error_ = hit2PositionError.x() / unit::cm;
  hit2_posy_error_ = hit2PositionError.y() / unit::cm;
  hit2_posz_error_ = hit2PositionError.z() / unit::cm;
  hit2_energy_error_ = event.Hit2EnergyError() / unit::keV;

  energy_reconstructed_ = event.IncidentEnergy() / unit::keV;

  flags_ = event.Flags();
  costheta_ = event.CosThetaE();
  dtheta_ = event.DeltaTheta();
  phi_ = event.PhiG();
  hitpattern_ = event.HitPattern();
  grade_ = event.Grade();
  likelihood_ = event.Likelihood();

  escape_flag_ = event.EscapeFlag();
  total_energy_deposit_ = event.TotalEnergyDeposit() / unit::keV;
  reconstructed_order_ = event.ReconstructedOrder();
  reconstruction_fraction_ = event.ReconstructionFraction();
  
  cetree_->Fill();
}

void ComptonEventTreeIO::fillEvents(const int64_t eventID,
                                    const std::vector<BasicComptonEvent_sptr>& events)
{
  const int numCases = events.size();
  for (const auto& e: events) {
    fillEvent(eventID, numCases, *e);
  }
}

BasicComptonEvent ComptonEventTreeIO::retrieveEvent() const
{
  BasicComptonEvent event;
  retrieveEvent(event);
  return event;
}

void ComptonEventTreeIO::retrieveEvent(BasicComptonEvent& event) const
{
  event.setEventID(eventid_);
  event.setNumberOfHits(num_hits_);
  
  event.setHit1ID(hit1_id_);
  event.setHit1Process(hit1_process_);
  event.setHit1DetectorChannelID(DetectorBasedChannelID(hit1_detector_));
  event.setHit1ReadoutChannelID(ReadoutBasedChannelID(hit1_readout_module_,
                                                      hit1_section_,
                                                      hit1_channel_));
  event.setHit1Voxel(hit1_pixelx_, hit1_pixely_, hit1_pixelz_);
  event.setHit1Time(hit1_time_ * unit::second);
  event.setHit1Position(hit1_posx_ * unit::cm, hit1_posy_ * unit::cm, hit1_posz_ * unit::cm);
  event.setHit1Energy(hit1_energy_ * unit::keV);
  event.setHit1TimeError(hit1_time_error_ * unit::second);
  event.setHit1PositionError(hit1_posx_error_ * unit::cm, hit1_posy_error_ * unit::cm, hit1_posz_error_ * unit::cm);
  event.setHit1EnergyError(hit1_energy_error_ * unit::keV);

  event.setHit2ID(hit2_id_);
  event.setHit2Process(hit2_process_);
  event.setHit2DetectorChannelID(DetectorBasedChannelID(hit2_detector_));
  event.setHit2ReadoutChannelID(ReadoutBasedChannelID(hit2_readout_module_,
                                                      hit2_section_,
                                                      hit2_channel_));
  event.setHit2Voxel(hit2_pixelx_, hit2_pixely_, hit2_pixelz_);
  event.setHit2Time(hit2_time_ * unit::second);
  event.setHit2Position(hit2_posx_ * unit::cm, hit2_posy_ * unit::cm, hit2_posz_ * unit::cm);
  event.setHit2Energy(hit2_energy_ * unit::keV);
  event.setHit2TimeError(hit2_time_error_ * unit::second);
  event.setHit2PositionError(hit2_posx_error_ * unit::cm, hit2_posy_error_ * unit::cm, hit2_posz_error_ * unit::cm);
  event.setHit2EnergyError(hit2_energy_error_ * unit::keV);

  // Currently, BasicComptonEvent does not have a correspoing property
  // event.setEnergyReconstructed(energy_reconstructed_ * unit::keV);
  
  event.setFlags(flags_);
  event.setHitPattern(hitpattern_);
  event.setGrade(grade_);
  event.setLikelihood(likelihood_);

  event.setEscapeFlag(escape_flag_);
  event.setTotalEnergyDeposit(total_energy_deposit_ * unit::keV);
  event.setReconstructedOrder(reconstructed_order_);
  event.setReconstructionFraction(reconstruction_fraction_);
}

} /* namespace comptonsoft */
