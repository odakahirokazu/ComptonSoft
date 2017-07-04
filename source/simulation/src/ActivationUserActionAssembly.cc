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

#include "ActivationUserActionAssembly.hh"

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

#include "ActivationStackingAction.hh"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

ActivationUserActionAssembly::ActivationUserActionAssembly()
  : m_AnalysisManager(G4RootAnalysisManager::Instance()),
    m_FilenameBase("activation"),
    m_DetectionByGeneration(true),
    m_ProcessesToDetect{"protonInelastic"},
    m_LifetimeLimit(1.0e-3*unit::second),
    m_InitialEnergy(0.0)
{
  add_alias("ActivationUserActionAssembly");
}

ActivationUserActionAssembly::~ActivationUserActionAssembly() = default;

ANLStatus ActivationUserActionAssembly::mod_startup()
{
  register_parameter(&m_FilenameBase, "output_filename_base");
  register_parameter(&m_DetectionByGeneration, "detection_by_generation");
  register_parameter(&m_ProcessesToDetect, "processes_to_detect");
  register_parameter(&m_LifetimeLimit, "lifetime_limit", unit::second, "s");
  return AS_OK;
}

void ActivationUserActionAssembly::createUserActions()
{
  setStackingAction(new ActivationStackingAction);
}

void ActivationUserActionAssembly::RunActionAtBeginning(const G4Run*)
{
  const G4String filename(m_FilenameBase+".root");
  m_AnalysisManager->SetNtupleDirectoryName("activation");
  m_AnalysisManager->OpenFile(filename);
  
  m_AnalysisManager->CreateNtuple("ritree", "Radioactive isotope tree");
  m_AnalysisManager->CreateNtupleIColumn("Z");
  m_AnalysisManager->CreateNtupleIColumn("A");
  m_AnalysisManager->CreateNtupleDColumn("E");
  m_AnalysisManager->CreateNtupleIColumn("floating_level");
  m_AnalysisManager->CreateNtupleIColumn("volume");
  m_AnalysisManager->CreateNtupleFColumn("posx");
  m_AnalysisManager->CreateNtupleFColumn("posy");
  m_AnalysisManager->CreateNtupleFColumn("posz");
  m_AnalysisManager->CreateNtupleDColumn("ini_energy");
}

void ActivationUserActionAssembly::RunActionAtEnd(const G4Run* run)
{
  OutputVolumeInfo(m_FilenameBase+".volume.dat");
  OutputSummary(m_FilenameBase+".summary.dat", run->GetNumberOfEvent());
  
  m_AnalysisManager->Write();
  m_AnalysisManager->CloseFile();
}

void ActivationUserActionAssembly::TrackActionAtBeginning(const G4Track* track)
{
  StandardUserActionAssembly::TrackActionAtBeginning(track);

  if (track->GetTrackID() == 1) {
    SetInitialEnergy(track->GetKineticEnergy());
  }
}

void ActivationUserActionAssembly::SteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();
  const G4String processName = process->GetProcessName();

  if (std::find(std::begin(m_ProcessesToDetect),
                std::end(m_ProcessesToDetect),
                processName) != std::end(m_ProcessesToDetect)) {
    const G4VTouchable* hist = step->GetPreStepPoint()->GetTouchable();
    const G4ThreeVector position = step->GetPreStepPoint()->GetPosition();

    if (m_DetectionByGeneration) {
      const std::vector<const G4Track*>* secondaries
        = step->GetSecondaryInCurrentStep();
      for (const G4Track* secondaryTrack: *secondaries) {
        G4ParticleDefinition* particle = secondaryTrack->GetDefinition();
        const int massNumber = particle->GetAtomicMass();
        if (massNumber > 4) { // heavier than He4 (alpha)
          G4Ions* nucleus = dynamic_cast<G4Ions*>(particle);
          if (nucleus) {
            Fill(nucleus, hist, position.x(), position.y(), position.z());
          }
        }
      }
    }
    else { // detection by decay
      G4ParticleDefinition* particle = track->GetDefinition();
      const int massNumber = particle->GetAtomicMass();
      if (massNumber > 4) { // heavier than He4 (alpha)
        G4Ions* nucleus = dynamic_cast<G4Ions*>(particle);
        if (nucleus) {
          if (nucleus->GetPDGLifeTime() >= m_LifetimeLimit) {
            Fill(nucleus, hist, position.x(), position.y(), position.z());
            track->SetTrackStatus(fKillTrackAndSecondaries);
          }
        }
      }
    }
  }
}

int ActivationUserActionAssembly::NumberOfVolumes()
{
  return m_VolumeArray.size();
}

std::string ActivationUserActionAssembly::VolumeName(int index)
{
  return m_VolumeArray.at(index);
}

void ActivationUserActionAssembly::Fill(const G4Ions* nucleus,
                                        const G4VTouchable* touchable,
                                        double posx, double posy, double posz)
{
  const int Z = nucleus->GetAtomicNumber();
  const int A = nucleus->GetAtomicMass();
  const double Energy = nucleus->GetExcitationEnergy();
  const int floatingLevel = nucleus->GetFloatLevelBaseIndex();
  
  G4String volumeName;
  for (int d=touchable->GetHistoryDepth(); d>=0; d--) {
    G4String currentVolumeName = touchable->GetVolume(d)->GetName();
    volumeName += '/';
    volumeName += currentVolumeName;
  }

  int volumeIndex = 0;
  volume_map_t::iterator volumeIter = m_VolumeMap.find(volumeName);
  if (volumeIter != m_VolumeMap.end()) {
    volumeIndex = (*volumeIter).second;
  }
  else {
    volumeIndex = NumberOfVolumes();
    m_VolumeMap[volumeName] = volumeIndex;
    m_VolumeArray.push_back(volumeName);
    m_RIMapVector.resize(volumeIndex+1);
  }

  // fill ntuple
  m_AnalysisManager->FillNtupleIColumn(0, Z);
  m_AnalysisManager->FillNtupleIColumn(1, A);
  m_AnalysisManager->FillNtupleDColumn(2, Energy/unit::keV);
  m_AnalysisManager->FillNtupleIColumn(3, floatingLevel);
  m_AnalysisManager->FillNtupleIColumn(4, volumeIndex);
  m_AnalysisManager->FillNtupleFColumn(5, posx/unit::cm);
  m_AnalysisManager->FillNtupleFColumn(6, posy/unit::cm);
  m_AnalysisManager->FillNtupleFColumn(7, posz/unit::cm);
  m_AnalysisManager->FillNtupleDColumn(8, m_InitialEnergy/unit::keV);
  m_AnalysisManager->AddNtupleRow();
  
  // fill data map
  data_map_t& data = m_RIMapVector[volumeIndex];
  const int64_t isotopeID = IsotopeInfo::makeID(Z, A, Energy, floatingLevel);
  data_map_t::iterator it = data.find(isotopeID);
  if (it != data.end()) {
    (*it).second.add1();
  }
  else {
    IsotopeInfo isotope(Z, A, Energy, floatingLevel);
    isotope.add1();
    data[isotopeID] = isotope;
  }
}

void ActivationUserActionAssembly::OutputVolumeInfo(const std::string& filename)
{
  std::ofstream fout(filename.c_str());
  size_t numVolume = NumberOfVolumes();
  for (size_t i=0; i<numVolume; i++) {
    fout << std::setw(7) << i << " " << VolumeName(i) << '\n';
  }
  fout.close();
}

void ActivationUserActionAssembly::OutputSummary(const std::string& filename,
                                                 int numberOfRun)
{
  std::ofstream fout(filename.c_str());
  fout << "NumberOfEvents " << numberOfRun << '\n' << std::endl;
  
  for (size_t i=0; i<m_RIMapVector.size(); i++) {
    fout << "Volume[" << i << "] " << VolumeName(i) << std::endl;
    
    data_map_t& data = m_RIMapVector[i];
    for (data_map_t::iterator it=data.begin(); it!=data.end(); ++it) {
      int64_t isotopeID = (*it).first;
      IsotopeInfo isotope = (*it).second;
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
