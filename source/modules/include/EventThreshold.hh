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

// 2011-02-09 Hirokazu Odaka

#ifndef COMPTONSOFT_EventThreshold_H
#define COMPTONSOFT_EventThreshold_H 1

#include <vector>

#include "BasicModule.hh"

#include "DetectorGroup.hh"
#include "HitPattern.hh"
#include "CSHitCollection.hh"
#include "ComptonEvent.hh"

namespace comptonsoft {

class EventThreshold : public anl::BasicModule
{
  DEFINE_ANL_MODULE(EventThreshold, 1.0);
public:
  EventThreshold();
  ~EventThreshold() {}

  anl::ANLStatus mod_startup();
  virtual anl::ANLStatus mod_com();
  virtual anl::ANLStatus mod_init();
  virtual anl::ANLStatus mod_ana();
  virtual anl::ANLStatus mod_exit();

protected:
  const comptonsoft::DetectorGroup* get_detector_group(const std::string& name);
  bool load_detector_groups();
private:
  CSHitCollection* hit_collection;

  std::string dg_filename;
  std::vector<comptonsoft::DetectorGroup> detector_groups;
  std::vector<std::pair<std::string, double> > threshold_vector;
};

}

#endif /* COMPTONSOFT_EventThreshold_H */
