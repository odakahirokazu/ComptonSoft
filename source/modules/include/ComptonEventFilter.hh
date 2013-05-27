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

// Hiro Odaka

#ifndef COMPTONSOFT_ComptonEventFilter_H
#define COMPTONSOFT_ComptonEventFilter_H 1

#include <vector>
#include <boost/function.hpp>

#include "BasicModule.hh"

#include "globals.hh"

#include "TwoHitComptonEvent.hh"
#include "EventReconstruction.hh"
#include "HitPattern.hh"

namespace comptonsoft {

class ComptonEventFilter : public anl::BasicModule
{
  DEFINE_ANL_MODULE(ComptonEventFilter, 3.1);
public:
  ComptonEventFilter();
  ~ComptonEventFilter() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();

  void define_condition();
  void add_condition(const std::string& type,
                     double min_value, double max_value);
    
private:
  EventReconstruction* EventReconstruction_ptr;
  std::string hitpat_name;
  comptonsoft::HitPattern hitpat;
  bool hitpat_selection;

  std::vector< std::vector< boost::function<bool (const comptonsoft::TwoHitComptonEvent&)> > > conditions_vector;
};

}

#endif /* COMPTONSOFT_ComptonEventFilter_H */
