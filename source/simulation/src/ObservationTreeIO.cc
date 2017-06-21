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

#include "ObservationTreeIO.hh"
#include <sstream>
#include "G4SystemOfUnits.hh"
#include "TTree.h"
#include "CSException.hh"

namespace comptonsoft
{

ObservationTreeIO::ObservationTreeIO()
  : tree_(nullptr)
{
}

ObservationTreeIO::~ObservationTreeIO() = default;

void ObservationTreeIO::defineBranches()
{
  tree_->Branch("eventid",  &eventid_,  "eventid/L");
  tree_->Branch("num",      &num_,      "num/I");
  tree_->Branch("trackid",  &trackid_,  "trackid/I");
  tree_->Branch("particle", &particle_, "particle/I");
  tree_->Branch("time",     &time_,     "time/D");
  tree_->Branch("posx",     &posx_,     "posx/F");
  tree_->Branch("posy",     &posy_,     "posy/F");
  tree_->Branch("posz",     &posz_,     "posz/F");
  tree_->Branch("energy",   &energy_,   "energy/F");
  tree_->Branch("dirx",     &dirx_,     "dirx/F");
  tree_->Branch("diry",     &diry_,     "diry/F");
  tree_->Branch("dirz",     &dirz_,     "dirz/F");
  tree_->Branch("polarx",   &polarx_,   "polarx/F");
  tree_->Branch("polary",   &polary_,   "polary/F");
  tree_->Branch("polarz",   &polarz_,   "polarz/F");
}

void ObservationTreeIO::setBranchAddresses()
{
  tree_->SetBranchAddress("eventid",  &eventid_);
  tree_->SetBranchAddress("num",      &num_);
  tree_->SetBranchAddress("trackid",  &trackid_);
  tree_->SetBranchAddress("particle", &particle_);
  tree_->SetBranchAddress("time",     &time_);
  tree_->SetBranchAddress("posx",     &posx_);
  tree_->SetBranchAddress("posy",     &posy_);
  tree_->SetBranchAddress("posz",     &posz_);
  tree_->SetBranchAddress("energy",   &energy_);
  tree_->SetBranchAddress("dirx",     &dirx_);
  tree_->SetBranchAddress("diry",     &diry_);
  tree_->SetBranchAddress("dirz",     &dirz_);
  tree_->SetBranchAddress("polarx",   &polarx_);
  tree_->SetBranchAddress("polary",   &polary_);
  tree_->SetBranchAddress("polarz",   &polarz_);
}

void ObservationTreeIO::
fillParticles(const int64_t eventID,
              const std::vector<ObservedParticle_sptr>& particles)
{
  eventid_ = eventID;

  const std::size_t NumParticles = particles.size();
  num_ = static_cast<int32_t>(NumParticles);

  for (std::size_t i=0; i<NumParticles; i++) {
    const ObservedParticle_sptr& particle = particles[i];
    trackid_ = particle->trackid;
    particle_ = particle->particle;
    time_ = particle->time / second;
    posx_ = static_cast<float>(particle->position.x()/cm);
    posy_ = static_cast<float>(particle->position.y()/cm);
    posz_ = static_cast<float>(particle->position.z()/cm);
    energy_ = static_cast<float>(particle->energy/keV);
    dirx_ = static_cast<float>(particle->direction.x());
    diry_ = static_cast<float>(particle->direction.y());
    dirz_ = static_cast<float>(particle->direction.z());
    polarx_ = static_cast<float>(particle->polarization.x());
    polary_ = static_cast<float>(particle->polarization.y());
    polarz_ = static_cast<float>(particle->polarization.z());
    
    tree_->Fill();
  }
}

ObservedParticle_sptr
ObservationTreeIO::retrieveParticle() const
{
  ObservedParticle_sptr particle(new ObservedParticle);
  particle->trackid = trackid_;
  particle->particle = particle_;
  particle->time = time_ * second;
  particle->position.set(posx_*cm, posy_*cm, posz_*cm);
  particle->energy = energy_ * keV;
  particle->direction.set(dirx_, diry_, dirz_);
  particle->polarization.set(polarx_, polary_, polarz_);

  return particle;
}

std::pair<int64_t, std::vector<ObservedParticle_sptr>>
ObservationTreeIO::retrieveParticles(int64_t& entry,
                                     bool get_first_entry)
{
  std::vector<ObservedParticle_sptr> particles;

  if (get_first_entry) {
    tree_->GetEntry(entry);
  }

  const int64_t ThisEventID = eventid_;
  const std::size_t Num = getNumberOfParticles();
  for (std::size_t i=0; i<Num; i++) {
    if (i != 0) {
      tree_->GetEntry(entry+i);

      if (eventid_ != ThisEventID) {
        std::ostringstream message;
        message << "Error: inconsistent Event ID at "
                << ThisEventID << '\n';
        BOOST_THROW_EXCEPTION( CSException(message.str()) );
      }
    }
    
    ObservedParticle_sptr particle = retrieveParticle();
    particles.push_back(std::move(particle));
  }

  entry += Num;
  return std::make_pair(ThisEventID, particles);
}

} /* namespace comptonsoft */
