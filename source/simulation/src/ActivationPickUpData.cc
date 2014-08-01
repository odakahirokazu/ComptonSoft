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

#include "ActivationPickUpData.hh"

#include <fstream>
#include <iomanip>

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Run.hh"
#include "G4VProcess.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4RootAnalysisManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Ions.hh"

#include "ActivationStackingAction.hh"


namespace {
  
int64_t IsotopeID(int z, int a, double energy)
{
  int64_t id = 0;
  id += static_cast<int64_t>(z) * 1000000000 * 1000;
  id += static_cast<int64_t>(a) * 1000000000;
  id += static_cast<int64_t>(energy/eV) % 1000000000;
  
  /* Isotope ID :xxyyyzzzzzzzzz *
   * Z = xx		        *
   * A = yyy                    * 
   * E = zzz zzz zzz [eV]       */
  
  return id;
}

}

using namespace anl;
using namespace comptonsoft;


ActivationPickUpData::ActivationPickUpData()
  : m_AnalysisManager(0),
    m_FileNameBase("activation"),
    m_InitialEnergy(0.0)
{
  add_alias("ActivationPickUpData");
  SetStepActOn(true);
  
  m_AnalysisManager = G4RootAnalysisManager::Instance();
}


ActivationPickUpData::~ActivationPickUpData()
{
  if (m_AnalysisManager) delete m_AnalysisManager;
}


ANLStatus ActivationPickUpData::mod_startup()
{
  register_parameter(&m_FileNameBase, "output_filename_base");
  return AS_OK;
}


void ActivationPickUpData::CreateUserActions()
{
  SetStackingAction(new ActivationStackingAction);
}


void ActivationPickUpData::RunAct_begin(const G4Run*)
{
  G4String fileName(m_FileNameBase+".root");
  m_AnalysisManager->OpenFile(fileName);
  m_AnalysisManager->SetNtupleDirectoryName("activation");
  
  m_AnalysisManager->CreateNtuple("ritree", "ritree");
  m_AnalysisManager->CreateNtupleIColumn("Z");
  m_AnalysisManager->CreateNtupleIColumn("A");
  m_AnalysisManager->CreateNtupleDColumn("E");
  m_AnalysisManager->CreateNtupleIColumn("volume");
  m_AnalysisManager->CreateNtupleFColumn("posx");
  m_AnalysisManager->CreateNtupleFColumn("posy");
  m_AnalysisManager->CreateNtupleFColumn("posz");
  m_AnalysisManager->CreateNtupleDColumn("ini_energy");
}


void ActivationPickUpData::RunAct_end(const G4Run* aRun)
{
  OutputVolumeInfo(m_FileNameBase+"_volume.txt");
  OutputSummary(m_FileNameBase+"_summary.txt", aRun->GetNumberOfEvent());
  
  m_AnalysisManager->Write();
  m_AnalysisManager->CloseFile();
}


void ActivationPickUpData::TrackAct_begin(const G4Track* aTrack)
{
  StandardPickUpData::TrackAct_begin(aTrack);
  if (aTrack->GetTrackID() == 1) {
    SetInitialEnergy(aTrack->GetKineticEnergy());
  }
}


void ActivationPickUpData::StepAct(const G4Step* aStep, G4Track* aTrack)
{
  G4ParticleDefinition* particleType = aTrack->GetDefinition();
  static const G4VProcess* processRD = 0;
  if (!processRD) {
    const G4ProcessVector* vec =
    G4GenericIon::GenericIonDefinition()->GetProcessManager()->GetProcessList();
    for (int i=0; i<vec->size(); i++) {
      const G4VProcess* p = (*vec)[i];
      if (p->GetProcessName() == "RadioactiveDecay") {
        processRD = p;
        break;
      }
    }
    if (!processRD) {
      G4Exception("Activation", "Active0000", FatalException,
                  "GenericIon does not have RadioActiveDecay process.");
    }
  }
  
  const G4VProcess* proc = aStep->GetPostStepPoint()->GetProcessDefinedStep();
  if ( proc == processRD ) {
    G4Ions* nucleus = dynamic_cast<G4Ions*>(particleType);
    if (!nucleus) return; // Disregard triton
    
    const G4VTouchable* hist = aStep->GetPreStepPoint()->GetTouchable();
    if (hist->GetHistoryDepth() == 0) return; // if in the World
    
    G4ThreeVector position = aStep->GetPreStepPoint()->GetPosition();
    Fill(nucleus, hist, position.x(), position.y(), position.z());
    aTrack->SetTrackStatus(fKillTrackAndSecondaries);

#if 0
    double time = aStep->GetPostStepPoint()->GetGlobalTime();
    G4cout << "### Isotope: " << nucleus->GetParticleName();
    G4cout << "  ### Time: " << time/second << " [sec] ###" << G4endl;
#endif
  }
}


int ActivationPickUpData::NumberOfVolumes()
{
  return m_VolumeArray.size();
}


std::string ActivationPickUpData::VolumeName(int index)
{
  return m_VolumeArray.at(index);
}


void ActivationPickUpData::Fill(const G4Ions* nucleus,
                                const G4VTouchable* touchable,
                                double posx, double posy, double posz)
{
  const int Z = nucleus->GetAtomicNumber();
  const int A = nucleus->GetAtomicMass();
  const double Energy = nucleus->GetExcitationEnergy();
  
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
  m_AnalysisManager->FillNtupleDColumn(2, Energy);
  m_AnalysisManager->FillNtupleIColumn(3, volumeIndex);
  m_AnalysisManager->FillNtupleFColumn(4, posx);
  m_AnalysisManager->FillNtupleFColumn(5, posy);
  m_AnalysisManager->FillNtupleFColumn(6, posz);
  m_AnalysisManager->FillNtupleDColumn(7, m_InitialEnergy);
  m_AnalysisManager->AddNtupleRow();
  
  // fill data map
  data_map_t& data = m_RIMapVector[volumeIndex];
  int64_t isotopeID = IsotopeID(Z, A, Energy);
  data_map_t::iterator it = data.find(isotopeID);
  if (it != data.end()) {
    (*it).second.add1();
  }
  else {
    IsotopeInfo isotope(Z, A, Energy);
    isotope.add1();
    data[isotopeID] = isotope;
  }
}


void ActivationPickUpData::OutputVolumeInfo(const std::string& fileName)
{
  std::ofstream fout(fileName.c_str());
  size_t numVolume = NumberOfVolumes();
  for (size_t i=0; i<numVolume; i++) {
    fout << std::setw(7) << i << " " << VolumeName(i) << '\n';
  }
  fout.close();
}


void ActivationPickUpData::OutputSummary(const std::string& fileName,
                                         int numberOfRun)
{
  std::ofstream fout(fileName.c_str());
  fout << "TotalEvent: " << numberOfRun << '\n' << std::endl;
  
  for (size_t i=0; i<m_RIMapVector.size(); i++) {
    fout << "Volume[" << i << "] " << VolumeName(i) << std::endl;
    
    data_map_t& data = m_RIMapVector[i];
    for (data_map_t::iterator it=data.begin(); it!=data.end(); ++it) {
      int64_t isotopeID = (*it).first;
      IsotopeInfo isotope = (*it).second;
      fout << std::setw(14) << isotopeID << " "
           << std::setw(3) << isotope.Z() << " "
           << std::setw(3) << isotope.A() << " "
           << std::setw(9) << std::setprecision(3) << isotope.Energy()/keV << " "
           << std::setw(12) << isotope.Count() << '\n';
    }
    fout << std::endl;
  }
  fout.close();
}
