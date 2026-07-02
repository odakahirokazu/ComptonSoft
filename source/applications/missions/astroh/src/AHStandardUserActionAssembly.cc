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

#include "AHStandardUserActionAssembly.hh"

#include <fstream>
#include <iomanip>
#include <algorithm>

#include "AstroUnits.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4RootAnalysisManager.hh"

#include "CSHitCollection.hh"
#include "InitialInformation.hh"
#include "SimXIF.hh"
#include "EventReconstruction.hh"
#include "DetectorHit.hh"
#include "BasicComptonEvent.hh"

using namespace anlnext;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

AHStandardUserActionAssembly::AHStandardUserActionAssembly()
  : m_AnalysisManager(0),
    m_HitCollection(0),
    m_InitialInfo(0),
    m_SimXIF(0),
    m_EventReconstruction(0),
    m_FileName("output.root"),
    m_PolarizationEnable(false)
{
  add_alias("AHStandardUserActionAssembly");
  
  m_AnalysisManager = G4RootAnalysisManager::Instance();
}

AHStandardUserActionAssembly::~AHStandardUserActionAssembly()
{
  if (m_AnalysisManager) delete m_AnalysisManager;
}

ANLStatus AHStandardUserActionAssembly::mod_define()
{
  register_parameter(&m_FileName, "output");
  return AS_OK;
}

ANLStatus AHStandardUserActionAssembly::mod_initialize()
{
  get_module_NC("CSHitCollection", &m_HitCollection);
  get_module_IF("InitialInformation", &m_InitialInfo);
  if (exist_module("SimXIF")) {
    get_module_NC("SimXIF", &m_SimXIF);
  }

  if (exist_module("EventReconstruction")) {
    get_module_NC("EventReconstruction", &m_EventReconstruction);
  }
  
  define_evs("HitTree:Fill");
  define_evs("CompMode:DeltaTheta:GOOD");
  define_evs("CompMode:DeltaTheta:NG");

  if (evs("Polarization enable")) {
    m_PolarizationEnable = true;
  }

  return AS_OK;
}

ANLStatus AHStandardUserActionAssembly::mod_analyze()
{
  const std::vector<DetectorHit_sptr>& hitVec = m_HitCollection->getHits();

  int num = hitVec.size();
  if (num>0) { set_evs("HitTree:Fill"); }
  
  for (int i=0; i<num; i++) {
    Fill(i, num, hitVec[i]);
  }

  if (m_SimXIF) MakeEvent(hitVec);

  return AS_OK;
}

ANLStatus AHStandardUserActionAssembly::mod_finalize()
{
  if (m_SimXIF) m_SimXIF->outputEvents();
  
  return AS_OK;
}

void AHStandardUserActionAssembly::RunActionAtBeginning(const G4Run*)
{
  if (m_AnalysisManager==0) {
    std::cout << "Analysis manager was already deleted." << std::endl;
    off();
    return;
  }
  
  m_AnalysisManager->OpenFile(m_FileName);
  m_AnalysisManager->SetNtupleDirectoryName("hit_list");
  m_AnalysisManager->CreateNtuple("hittree", "hittree");

  m_AnalysisManager->CreateNtupleIColumn("eventid");    //  0
  m_AnalysisManager->CreateNtupleIColumn("seqnum");     //  1
  m_AnalysisManager->CreateNtupleIColumn("totalhit");   //  2

  m_AnalysisManager->CreateNtupleFColumn("realposx");   //  3
  m_AnalysisManager->CreateNtupleFColumn("realposy");   //  4
  m_AnalysisManager->CreateNtupleFColumn("realposz");   //  5
  m_AnalysisManager->CreateNtupleFColumn("posx");       //  6
  m_AnalysisManager->CreateNtupleFColumn("posy");       //  7
  m_AnalysisManager->CreateNtupleFColumn("posz");       //  8

  m_AnalysisManager->CreateNtupleDColumn("edep");       //  9
  m_AnalysisManager->CreateNtupleDColumn("e_pha");      // 10
  m_AnalysisManager->CreateNtupleDColumn("e_pi");       // 11

  m_AnalysisManager->CreateNtupleDColumn("time");       // 12
 
  m_AnalysisManager->CreateNtupleIColumn("detid");      // 13
  m_AnalysisManager->CreateNtupleIColumn("stripx");     // 14
  m_AnalysisManager->CreateNtupleIColumn("stripy");     // 15

  m_AnalysisManager->CreateNtupleDColumn("ini_energy"); // 16
  m_AnalysisManager->CreateNtupleDColumn("ini_dirx");   // 17
  m_AnalysisManager->CreateNtupleDColumn("ini_diry");   // 18
  m_AnalysisManager->CreateNtupleDColumn("ini_dirz");   // 19
  m_AnalysisManager->CreateNtupleDColumn("ini_time");   // 20
  m_AnalysisManager->CreateNtupleDColumn("ini_posx");   // 21
  m_AnalysisManager->CreateNtupleDColumn("ini_posy");   // 22
  m_AnalysisManager->CreateNtupleDColumn("ini_posz");   // 23
  
  if (m_PolarizationEnable) {
    m_AnalysisManager->CreateNtupleDColumn("ini_polarx"); // 24
    m_AnalysisManager->CreateNtupleDColumn("ini_polary"); // 25
    m_AnalysisManager->CreateNtupleDColumn("ini_polarz"); // 26
  }
}

void AHStandardUserActionAssembly::RunActionAtEnd(const G4Run*)
{
  if (m_AnalysisManager) {
    m_AnalysisManager->Write();
    m_AnalysisManager->CloseFile();
    delete m_AnalysisManager;
    m_AnalysisManager = 0;
  }
  on();
}

void AHStandardUserActionAssembly::Fill(int seqnum,
                                        int totalhit,
                                        const_DetectorHit_sptr hit)
{
  const int eventID = m_InitialInfo->EventID();
  
  const G4double iniEnergy = m_InitialInfo->InitialEnergy();
  const G4ThreeVector iniDir = m_InitialInfo->InitialDirection();
  const G4double  iniTime = m_InitialInfo->InitialTime();
  const G4ThreeVector iniPos = m_InitialInfo->InitialPosition();
  const G4ThreeVector iniPolar = m_InitialInfo->InitialPolarization();

  G4double realposx = hit->RealPositionX();
  G4double realposy = hit->RealPositionY();
  G4double realposz = hit->RealPositionZ();
  G4double posx = hit->PositionX();
  G4double posy = hit->PositionY();
  G4double posz = hit->PositionZ();
  
  G4double edep  = hit->EnergyDeposit();
  G4double e_pha = hit->PHA();
  G4double e_pi  = hit->EPI();
  
  G4double time1 = hit->Time();
  
  G4double detid = hit->DetectorID();
  G4double stripx = hit->PixelX();
  G4double stripy = hit->PixelY();

  // fill ntuple
  m_AnalysisManager->FillNtupleIColumn(0, eventID);
  m_AnalysisManager->FillNtupleIColumn(1, seqnum);
  m_AnalysisManager->FillNtupleIColumn(2, totalhit);

  m_AnalysisManager->FillNtupleFColumn(3, realposx);
  m_AnalysisManager->FillNtupleFColumn(4, realposy);
  m_AnalysisManager->FillNtupleFColumn(5, realposz);
  m_AnalysisManager->FillNtupleFColumn(6, posx);
  m_AnalysisManager->FillNtupleFColumn(7, posy);
  m_AnalysisManager->FillNtupleFColumn(8, posz);

  m_AnalysisManager->FillNtupleDColumn(9, edep);
  m_AnalysisManager->FillNtupleDColumn(10, e_pha);
  m_AnalysisManager->FillNtupleDColumn(11, e_pi);

  m_AnalysisManager->FillNtupleDColumn(12, time1);

  m_AnalysisManager->FillNtupleIColumn(13, detid);
  m_AnalysisManager->FillNtupleIColumn(14, stripx);
  m_AnalysisManager->FillNtupleIColumn(15, stripy);

  m_AnalysisManager->FillNtupleDColumn(16, iniEnergy);
  m_AnalysisManager->FillNtupleDColumn(17, iniDir.x());
  m_AnalysisManager->FillNtupleDColumn(18, iniDir.y());
  m_AnalysisManager->FillNtupleDColumn(19, iniDir.z());
  m_AnalysisManager->FillNtupleDColumn(20, iniTime);
  m_AnalysisManager->FillNtupleDColumn(21, iniPos.x());
  m_AnalysisManager->FillNtupleDColumn(22, iniPos.y());
  m_AnalysisManager->FillNtupleDColumn(23, iniPos.z());

  if (m_PolarizationEnable) {
    m_AnalysisManager->FillNtupleDColumn(24, iniPolar.x());
    m_AnalysisManager->FillNtupleDColumn(25, iniPolar.y());
    m_AnalysisManager->FillNtupleDColumn(26, iniPolar.z());
  }
  
  m_AnalysisManager->AddNtupleRow();
}

} /* namespace comptonsoft */

namespace {

double ThetaCutOfComptonMode(double energy)
{
  // by S. Watanabe
  double cut = 8.0 * unit::degree;
  if (energy < 300.0*unit::keV) { cut = (26.0 - energy/unit::keV * 0.06) * unit::degree; }
  return cut;
}

int findMaxPI(const std::vector<comptonsoft::DetectorHit_sptr>& hits)
{
  int index = -1;
  double x = -1.0;
  for (size_t i=0; i<hits.size(); i++) {
    double ePI = hits[i]->EPI();
    if (x < ePI) {
      x = ePI;
      index = i;
    }
  }
  return index;
}

} /* anonymous namespace */

namespace comptonsoft
{

void AHStandardUserActionAssembly::MakeEvent(const std::vector<DetectorHit_sptr>& hits)
{
  double energy(0.0), time1(0.0);
  int stripx(0), stripy(0);
  int detectorID(0);

  if (m_EventReconstruction) {
    const BasicComptonEvent& twoHitData = m_EventReconstruction->getComptonEvent();
    const double dtheta = std::abs(twoHitData.DeltaTheta());
    energy = twoHitData.TotalEnergy();

    if (dtheta > ThetaCutOfComptonMode(energy)) {
      set_evs("CompMode:DeltaTheta:NG");
      return;
    }

    set_evs("CompMode:DeltaTheta:GOOD");
    time1 = twoHitData.Hit1Time();
    stripx = twoHitData.Hit1Pixel().X();
    stripy = twoHitData.Hit1Pixel().Y();
    detectorID = twoHitData.Hit1DetectorID();
  }
  else {
    int i = findMaxPI(hits);
    if (i<0) return;

    DetectorHit_sptr hit(hits[i]);
    time1 = hit->Time();
    stripx = hit->PixelX();
    stripy = hit->PixelY();
    energy = hit->EPI();
    detectorID = hit->DetectorID();
  }

  m_SimXIF->addEvent(time1, energy, stripx, stripy, detectorID);
}

} /* namespace comptonsoft */
