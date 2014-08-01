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

#include "EventReconstruction.hh"

#include <fstream>
#include "globals.hh"

#include "DetectorHit.hh"
#include "NextCLI.hh"

using namespace comptonsoft;
using namespace anl;

EventReconstruction::EventReconstruction()
  : hit_collection(0),
    max_hit(2), ecut(false),
    energy_range0(0.0*keV), energy_range1(10.0*MeV),
    dg_filename("detector_group.txt"),
    source_distant(true),
    source_direction_x(0.0), source_direction_y(0.0), source_direction_z(1.0),
    source_position_x(0.0), source_position_y(0.0), source_position_z(0.0)
{
  add_alias("EventReconstruction");
}


ANLStatus EventReconstruction::mod_startup()
{
  register_parameter(&dg_filename, "detector_group");
  register_parameter(&max_hit, "maximum_hits_for_analysis");
  register_parameter(&ecut, "total_energy_cut");
  register_parameter(&energy_range0, "energy_min", keV, "keV");
  register_parameter(&energy_range1, "energy_max", keV, "keV");

  register_parameter(&source_distant, "source_distant");
  register_parameter(&source_direction_x, "source_direction_x");
  register_parameter(&source_direction_y, "source_direction_y");
  register_parameter(&source_direction_z, "source_direction_z");
  register_parameter(&source_position_x, "source_position_x", cm, "cm");
  register_parameter(&source_position_y, "source_position_y", cm, "cm");
  register_parameter(&source_position_z, "source_position_z", cm, "cm");

  init_bnk_evs();

  return AS_OK;
}


ANLStatus EventReconstruction::mod_com()
{
  ask_parameter("total_energy_cut", "Cut by total energy? (1:yes, 0:no)");
  hide_parameter("total_energy_cut");
  
  ask_parameter("source_distant", "Is the source infinitely distant? (1:yes, 0:no)");
  hide_parameter("source_distant");
  
  if (!ecut) {
    unregister_parameter("energy_min");
    unregister_parameter("energy_max");
  }
  
  if (source_distant) {
    unregister_parameter("source_position_x");
    unregister_parameter("source_position_y");
    unregister_parameter("source_position_z");
  }
  else {
    unregister_parameter("source_direction_x");
    unregister_parameter("source_direction_y");
    unregister_parameter("source_direction_z");
  }

  ask_parameters();
 
  return AS_OK;
}


ANLStatus EventReconstruction::mod_prepare()
{
  assign_source_information();

  if (dg_filename[0] == '0') {
    std::cout << "EventReconstruction: There is no detector group." << std::endl;
    return AS_OK;
  }

  if (!load_detector_groups()) {
    return AS_QUIT;
  }
  
  return AS_OK;
}


ANLStatus EventReconstruction::mod_init()
{
  GetANLModuleNC("CSHitCollection", &hit_collection);
  
  return AS_OK;
}


ANLStatus EventReconstruction::mod_ana()
{
  int numTimeGroup = hit_collection->NumTimeGroup();
  int timeGroup = numTimeGroup - 1;
  const std::vector<DetectorHit_sptr>& hitVec
    = hit_collection->GetHits(timeGroup);
  const int nHit = hitVec.size();

  bool veto = false;
  for (int i=0; i<nHit; i++) {
    if ((hitVec[i]->getFlag() & comptonsoft::BGO_VETO)) {
      veto = true;
      break;
    }
  }
  if (veto) {
    EvsSet("EventReconst:Veto");
    return AS_SKIP;
  }

  bool si_trig = false;
  for (int i=0; i<nHit; i++) {
    if ((hitVec[i]->getFlag() & comptonsoft::SI_TRIGGER)) {
      si_trig = true;
      break;
    }
  }
  if (si_trig) {
    EvsSet("EventReconst:SiTrigger");
  }
  
  bool cdte_trig = false;
  for (int i=0; i<nHit; i++) {
    if ((hitVec[i]->getFlag() & comptonsoft::CDTE_TRIGGER)) {
      cdte_trig = true;
      break;
    }
  }
  if (cdte_trig) {
    EvsSet("EventReconst:CdTeTrigger");
  }
  
  compton_event.clearHits();
  compton_event.setFlag(0);
  for (int i=0; i<nHit; i++) {
    DetectorHit_sptr hit = hitVec[i];
    compton_event.addHit(hit);
  }

  bool result = true;
  if (ecut) {
    result = compton_event.extractEventsByTotalEnergy(energy_range0, energy_range1);
    if (!result) {
      return AS_SKIP;
    }
  }
  
  const int nHitSelected = compton_event.NumberOfHits();
  if (nHitSelected == 0) {
    EvsSet("EventReconst:0Hit");
    return AS_SKIP;
  }
  else if (nHitSelected == 1) {
    EvsSet("EventReconst:1Hit");
  }
  else if (nHitSelected == 2) {
    EvsSet("EventReconst:2Hit");
  }
  else if (nHitSelected == 3) {
    EvsSet("EventReconst:3Hit");
  }
  else if (nHitSelected == 4) {
    EvsSet("EventReconst:4Hit");
  }
  else {
    EvsSet("EventReconst:ManyHit");
  }

  determine_hit_pattern(compton_event.getHits());
  
  if (Evs("EventReconst:2Hit")) {
    if (si_trig && cdte_trig) {
      EvsSet("EventReconst:2Hit:Si-CdTe");
    }
    else {
      if (si_trig) {
	EvsSet("EventReconst:2Hit:Si-Si");
      }
      else if (cdte_trig) {
	EvsSet("EventReconst:2Hit:CdTe-CdTe");
      }
    }
  }

  result = compton_event.reconstruct(max_hit);
  if (!result) {
    return AS_SKIP;
  }
    
  if (compton_event.NumberOfHits() == 2) {
    unsigned int twohitflag = compton_event.getHit(0)->getFlag() | compton_event.getHit(1)->getFlag();
  
    bool cdte_fluor = false;
#if 1
    if (compton_event.FluorescenceCut()) {
#else
    if (twohitflag & comptonsoft::CDTE_FLUOR) {
#endif
      cdte_fluor = true;
      EvsSet("EventReconst:CdTeFluor");
    }
    
    const double ELECTRON_MASS = 511.0 * keV;
    if (compton_event.TotalEnergy() >= 0.5*ELECTRON_MASS) {
      EvsSet("EventReconst:HighE");
      twohitflag |= comptonsoft::HI_ENERGY;
    }
    
    if (!compton_event.getReconstructedEvent().TimeOrder()) {
      EvsSet("EventReconst:BadTimeOrder");
      twohitflag |= comptonsoft::BAD_TIME_ORDER;
    }

    compton_event.addFlag(twohitflag);
  }
    
  EvsSet("EventReconst:Stored");
  
  return AS_OK;
}


ANLStatus EventReconstruction::mod_exit()
{
  std::cout << std::endl;
  std::cout << "Hit Pattern (EventReconstruction)" << std::endl;
  for (size_t i=0; i<hit_pattern_vec.size(); i++) {
    std::cout << hit_pattern_vec[i].Name() << "   "
	      << hit_pattern_counts[i] << std::endl;
  }

  return AS_OK;
}


const DetectorGroup* EventReconstruction::get_detector_group(const std::string& name)
{
  for (size_t i=0; i<detector_groups.size(); i++) {
    if (name == detector_groups[i].Name()) {
      return &(detector_groups[i]);
    }
  }

  return 0;
}


bool EventReconstruction::load_detector_groups()
{
  if (dg_filename[0] == '0') {
    std::cout << "EventReconstruction: There is no detector group." << std::endl;
    return true;
  }

  std::ifstream fin;
  fin.open(dg_filename.c_str());
  if (!fin) {
    std::cout << "EventReconstruction: file cannot open" << std::endl;
    return false;
  }

  DetectorGroup* dg = 0;

  while (1) {
    int detid;
    std::string str;
    char buf[256];
    fin.getline(buf, 256);
    if(fin.eof()) { break; }
    if (buf[0] == '#') { continue; }

    std::istringstream iss(buf);
    iss >> str;
    
    if (str == "GROUP") {
      iss >> str;
 
      int vecsize = detector_groups.size();
      detector_groups.resize(vecsize+1);
      dg = &detector_groups[vecsize];
      dg->setName(str);
    }
    else if (str == "ID") {
      while (iss >> detid) {
	if (dg) { dg->add(detid); }
      }
    }
    else if (str == "HITPATTERN") {
      HitPattern hitpat;
      iss >> str;
      hitpat.setName(str);
      iss >> str;
      hitpat.setShortName(str);
      while (iss >> str) {
	hitpat.add(*get_detector_group(str));
      }
      hit_pattern_vec.push_back(hitpat);
      hit_pattern_flags.push_back(0);
      hit_pattern_counts.push_back(0);
    }
  }
  
  for (size_t i=0; i<detector_groups.size(); i++) {
    detector_groups[i].print();
  }
  
  return true;
}


void EventReconstruction::init_bnk_evs()
{
  EvsDef("EventReconst:Veto");
  EvsDef("EventReconst:SiTrigger");
  EvsDef("EventReconst:CdTeTrigger");
  EvsDef("EventReconst:0Hit");
  EvsDef("EventReconst:1Hit");
  EvsDef("EventReconst:2Hit");
  EvsDef("EventReconst:3Hit");
  EvsDef("EventReconst:4Hit");
  EvsDef("EventReconst:ManyHit");

  EvsDef("EventReconst:2Hit:Si-Si");
  EvsDef("EventReconst:2Hit:Si-CdTe");
  EvsDef("EventReconst:2Hit:CdTe-CdTe");
  EvsDef("EventReconst:CdTeFluor");
  EvsDef("EventReconst:HighE");
  EvsDef("EventReconst:BadTimeOrder");
  EvsDef("EventReconst:Stored");
}


void EventReconstruction::determine_hit_pattern(const std::vector<DetectorHit_sptr>& hitvec)
{
  for (unsigned int i=0; i<hit_pattern_vec.size(); i++) {
    if (hit_pattern_vec[i].match(hitvec)) {
      hit_pattern_flags[i] = 1;
      hit_pattern_counts[i]++;
    }
    else {
      hit_pattern_flags[i] = 0;
    }
  }
}


void EventReconstruction::determine_hit_pattern(const std::vector<DetectorHit*>& hitvec)
{
  for (unsigned int i=0; i<hit_pattern_vec.size(); i++) {
    if (hit_pattern_vec[i].match(hitvec)) {
      hit_pattern_flags[i] = 1;
      hit_pattern_counts[i]++;
    }
    else {
      hit_pattern_flags[i] = 0;
    }
  }
}


void EventReconstruction::determine_hit_pattern(const std::vector<int>& idvec)
{
  for (unsigned int i=0; i<hit_pattern_vec.size(); i++) {
    if (hit_pattern_vec[i].match(idvec)) {
      hit_pattern_flags[i] = 1;
      hit_pattern_counts[i]++;
    }
    else {
      hit_pattern_flags[i] = 0;
    }
  }
}


void EventReconstruction::assign_source_information()
{
  if (source_distant) {
    compton_event.setSourceDirection(source_direction_x,
                                     source_direction_y,
                                     source_direction_z);
  }
  else {
    compton_event.setSourcePosition(source_position_x,
                                    source_position_y,
                                    source_position_z);
  }
}
