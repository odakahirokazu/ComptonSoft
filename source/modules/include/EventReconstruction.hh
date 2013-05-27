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

// 2008-12-15 Hirokazu Odaka

#ifndef COMPTONSOFT_EventReconstruction_H
#define COMPTONSOFT_EventReconstruction_H 1

#include <vector>

#include "BasicModule.hh"

#include "DetectorGroup.hh"
#include "HitPattern.hh"
#include "CSHitCollection.hh"
#include "ComptonEvent.hh"

namespace comptonsoft {

class EventReconstruction : public anl::BasicModule
{
  DEFINE_ANL_MODULE(EventReconstruction, 1.3)
public:
  EventReconstruction();
  ~EventReconstruction() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_prepare();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_exit();

  ComptonEvent& GetComptonEvent() { return compton_event; }
  const TwoHitComptonEvent& GetTwoHitData()
  { return compton_event.getReconstructedEvent(); }
  
  std::vector<HitPattern>& GetHitPatternVector() { return hit_pattern_vec; }
  int HitPatternFlag(int index) { return hit_pattern_flags[index]; }

protected:
  const DetectorGroup* get_detector_group(const std::string& name);
  void com_base();
  bool load_detector_groups();
  void init_bnk_evs();
  
  void determine_hit_pattern(const std::vector<DetectorHit_sptr>& hitvec);
  void determine_hit_pattern(const std::vector<DetectorHit*>& hitvec);
  void determine_hit_pattern(const std::vector<int>& idvec);
  void assign_source_information();

  ComptonEvent compton_event;

private:
  CSHitCollection* hit_collection;

  int min_hit;
  int max_hit;
  bool ecut;
  double energy_range0;
  double energy_range1;

  std::string dg_filename;
  std::vector<DetectorGroup> detector_groups;

  std::vector<HitPattern> hit_pattern_vec;
  std::vector<int> hit_pattern_flags;
  std::vector<int> hit_pattern_counts;

  bool source_distant;
  double source_direction_x;
  double source_direction_y;
  double source_direction_z;
  double source_position_x;
  double source_position_y;
  double source_position_z;
};

}

#endif /* COMPTONSOFT_EventReconstruction_H */
