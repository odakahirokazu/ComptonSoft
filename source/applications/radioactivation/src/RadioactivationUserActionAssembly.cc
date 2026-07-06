/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#include "RadioactivationUserActionAssembly.hh"

#include <fstream>
#include <iterator>
#include <algorithm>
#include <boost/format.hpp>

#include "AstroUnits.hh"
#include "G4Run.hh"
#include "G4VProcess.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4RootAnalysisManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Ions.hh"

#include "RadioactivationEventStore.hh"
#include "RadioactivationStackingAction.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

RadioactivationUserActionAssembly::RadioactivationUserActionAssembly()
  : filename_base_("radioactivation"),
    detection_by_generation_(true),
    processes_to_detect_{"protonInelastic"},
    lifetime_limit_(1.0e-3*unit::second),
    mutex_(std::make_shared<std::mutex>()),
    total_event_number_ptr_(std::make_shared<int>(0)),
    volume_map_(std::make_shared<volume_map_t>()),
    volume_vector_(std::make_shared<std::vector<std::string>>()),
    data_map_vector_(std::make_shared<std::vector<data_map_t>>())
{
  add_alias("RadioactivationUserActionAssembly");
}

RadioactivationUserActionAssembly::~RadioactivationUserActionAssembly() = default;

ANLStatus RadioactivationUserActionAssembly::mod_define()
{
  StandardUserActionAssembly::mod_define();

  define_parameter("output_filename_base", &mod_class::filename_base_);
  define_parameter("detection_by_generation", &mod_class::detection_by_generation_);
  define_parameter("processes_to_detect", &mod_class::processes_to_detect_);
  define_parameter("lifetime_limit", &mod_class::lifetime_limit_, unit::second, "s");

  return AS_OK;
}

ANLStatus RadioactivationUserActionAssembly::mod_initialize()
{
  StandardUserActionAssembly::mod_initialize();

  get_module_NC("RadioactivationEventStore", &event_store_);

  return AS_OK;
}

ANLStatus RadioactivationUserActionAssembly::mod_finalize()
{
  output_volume_info(filename_base_+".volume.dat");
  output_summary(filename_base_+".summary.dat");

  return StandardUserActionAssembly::mod_finalize();
}

void RadioactivationUserActionAssembly::run_action_at_end(const G4Run* run)
{
  *total_event_number_ptr_ += run->GetNumberOfEvent();
  StandardUserActionAssembly::run_action_at_end(run);
}

void RadioactivationUserActionAssembly::stepping_action(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();
  const G4String process_name = process->GetProcessName();

  if (std::find(std::begin(processes_to_detect_),
                std::end(processes_to_detect_),
                process_name) != std::end(processes_to_detect_)) {
    const G4TouchableHistory* hist = step->GetPreStepPoint()->GetTouchable();
    const G4ThreeVector position = step->GetPreStepPoint()->GetPosition();

    if (detection_by_generation_) {
      const std::vector<const G4Track*>* secondaries
        = step->GetSecondaryInCurrentStep();
      for (const G4Track* secondary_track: *secondaries) {
        G4ParticleDefinition* particle = secondary_track->GetDefinition();
        const int mass_number = particle->GetAtomicMass();
        if (mass_number > 4) { // heavier than He4 (alpha)
          G4Ions* nucleus = dynamic_cast<G4Ions*>(particle);
          if (nucleus) {
            fill(nucleus, hist, position);
          }
        }
      }
    }
    else { // detection by decay
      G4ParticleDefinition* particle = track->GetDefinition();
      const int mass_number = particle->GetAtomicMass();
      if (mass_number > 4) { // heavier than He4 (alpha)
        G4Ions* nucleus = dynamic_cast<G4Ions*>(particle);
        if (nucleus) {
          if (nucleus->GetPDGLifeTime() >= lifetime_limit_) {
            fill(nucleus, hist, position);
            track->SetTrackStatus(fKillTrackAndSecondaries);
          }
        }
      }
    }
  }
}

G4UserStackingAction* RadioactivationUserActionAssembly::create_stacking_action() const
{
  return new RadioactivationStackingAction;
}

void RadioactivationUserActionAssembly::fill(const G4Ions* nucleus,
                                        const G4TouchableHistory* touchable,
                                        const G4ThreeVector& position)
{
  const int Z = nucleus->GetAtomicNumber();
  const int A = nucleus->GetAtomicMass();
  const double energy = nucleus->GetExcitationEnergy();
  const int floatingLevel = nucleus->GetFloatLevelBaseIndex();
  const IsotopeInfo isotope(Z, A, energy, floatingLevel);

  std::string volume_name;
  for (int d=touchable->GetHistoryDepth(); d>=0; d--) {
    std::string current_volume_name = touchable->GetVolume(d)->GetName();
    volume_name += '/';
    volume_name += current_volume_name;
  }

  const int volume_index = analyze_volume_info(volume_name, isotope);
  event_store_->insert_radioactivation(getCurrentEventID(), std::make_tuple(isotope, volume_index, position));
}

int RadioactivationUserActionAssembly::analyze_volume_info(const std::string& volume_name, const IsotopeInfo& isotope)
{
  std::lock_guard<std::mutex> lock(*mutex_);

  int volume_index = 0;
  volume_map_t::iterator volume_iter = volume_map_->find(volume_name);
  if (volume_iter != volume_map_->end()) {
    volume_index = (*volume_iter).second;
  }
  else {
    volume_index = (*volume_map_).size();
    (*volume_map_)[volume_name] = volume_index;
    volume_vector_->push_back(volume_name);
    data_map_vector_->resize(volume_index+1);
  }

  // fill data map
  data_map_t& data = (*data_map_vector_)[volume_index];
  const int64_t isotopeID = isotope.IsotopeID();
  data_map_t::iterator it = data.find(isotopeID);
  if (it != data.end()) {
    (*it).second.add1();
  }
  else {
    IsotopeInfo isotope_new(isotope);
    isotope_new.add1();
    data[isotopeID] = isotope_new;
  }

  return volume_index;
}

void RadioactivationUserActionAssembly::output_volume_info(const std::string& filename)
{
  std::ofstream fout(filename.c_str());
  for (auto pair: *volume_map_) {
    fout << std::setw(7) << pair.first << " " << pair.second << '\n';
  }
  fout.close();
}

void RadioactivationUserActionAssembly::output_summary(const std::string& filename)
{
  std::ofstream fout(filename.c_str());
  fout << "NumberOfEvents " << *total_event_number_ptr_ << '\n' << std::endl;

  std::vector<data_map_t>& data_vector = *data_map_vector_;
  for (size_t i=0; i<data_vector.size(); i++) {
    const std::string volume_name = volume_vector_->at(i);
    data_map_t& data_map = data_map_vector_->at(i);

    fout << "Volume[" << i << "] " << volume_name << std::endl;

    for (data_map_t::iterator it=data_map.begin(); it!=data_map.end(); ++it) {
      const int64_t isotopeID = (*it).first;
      const IsotopeInfo& isotope = (*it).second;
      fout << (boost::format("Isotope %16d %3d %3d %15.9e %2d %15d\n")
               % isotopeID
               % isotope.Z()
               % isotope.A()
               % (isotope.Energy()/unit::keV)
               % isotope.FloatingLevel()
               % isotope.Counts());
    }
    fout << std::endl;
  }
  fout.close();
}

} /* namespace comptonsoft */
