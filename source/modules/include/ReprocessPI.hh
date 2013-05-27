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

// 2011-02-16 Hirokazu Odaka
// 2011-09-14 Hirokazu Odaka

#ifndef COMPTONSOFT_ReprocessPI_H
#define COMPTONSOFT_ReprocessPI_H 1

#include <vector>
#include <map>
#include <boost/tuple/tuple.hpp>

#include "BasicModule.hh"

#include "DetectorGroup.hh"
#include "HitPattern.hh"
#include "CSHitCollection.hh"
#include "ComptonEvent.hh"


namespace comptonsoft {

class ReprocessPI : public anl::BasicModule
{
  DEFINE_ANL_MODULE(ReprocessPI, 2.0);
public:
  ReprocessPI();
  ~ReprocessPI() {}

  anl::ANLStatus mod_startup();
  //  virtual anl::ANLStatus mod_com();
  virtual anl::ANLStatus mod_init();
  virtual anl::ANLStatus mod_ana();
  
protected:
  const comptonsoft::DetectorGroup* get_detector_group(const std::string& name);
  bool load_detector_groups();
private:
  CSHitCollection* hit_collection;

  std::string dg_filename;
  std::vector<comptonsoft::DetectorGroup> detector_groups;

  typedef boost::tuple<double, double, double, double> map_value_type;
  typedef std::map<std::string, map_value_type> map_type;
  map_type param_map;

  double m_Factor0;
  double m_Noise00;
  double m_Noise01;
  double m_Noise02;
};

}

#endif /* COMPTONSOFT_ReprocessPI_H */
