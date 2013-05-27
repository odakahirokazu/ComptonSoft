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

#ifndef COMPTONSOFT_SelectHitPattern_H
#define COMPTONSOFT_SelectHitPattern_H 1

#include <vector>

#include "BasicModule.hh"

#include "DetectorGroup.hh"
#include "HitPattern.hh"
#include "CSHitCollection.hh"

namespace comptonsoft {

/**
 * ANL module to determine hit patterns of events.
 * 
 * @author Hirokazu Odaka
 & @date 2012-06-20
*/
class SelectHitPattern : public anl::BasicModule
{
  DEFINE_ANL_MODULE(SelectHitPattern, 1.0);
public:
  SelectHitPattern();
  ~SelectHitPattern() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_prepare();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_exit();

  std::vector<comptonsoft::HitPattern>& GetHitPatternVector() { return hit_pattern_vec; }
  int HitPatternFlag(int index) { return hit_pattern_flags[index]; }

protected:
  const comptonsoft::DetectorGroup* get_detector_group(const std::string& name);
  bool load_detector_groups();
  
  void determine_hit_pattern(const std::vector<DetectorHit_sptr>& hitvec);
  void determine_hit_pattern(const std::vector<DetectorHit*>& hitvec);
  void determine_hit_pattern(const std::vector<int>& idvec);

private:
  CSHitCollection* hit_collection;

  std::string dg_filename;
  std::vector<comptonsoft::DetectorGroup> detector_groups;

  std::vector<comptonsoft::HitPattern> hit_pattern_vec;
  std::vector<int> hit_pattern_flags;
  std::vector<int> hit_pattern_counts;
};

}

#endif /* COMPTONSOFT_SelectHitPattern_H */
