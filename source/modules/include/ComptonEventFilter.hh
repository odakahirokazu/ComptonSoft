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

#ifndef COMPTONSOFT_ComptonEventFilter_H
#define COMPTONSOFT_ComptonEventFilter_H 1

#include "BasicModule.hh"

#include <functional>
#include "HitPattern.hh"
#include "VCSModule.hh"

namespace comptonsoft {

class BasicComptonEvent;
class EventReconstruction;

/**
 *
 * @author Hirokazu Odaka
 * @date 2011-xx-xx
 * @date 2014-11-26
 * @date 2015-10-10 | derived from VCSModule
 */
class ComptonEventFilter : public VCSModule
{
  DEFINE_ANL_MODULE(ComptonEventFilter, 3.3);
public:
  ComptonEventFilter();
  ~ComptonEventFilter() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();

  void define_condition();
  void add_condition(const std::string& type,
                     double minValue,
                     double maxValue);
    
private:
  EventReconstruction* m_EventReconstruction;
  std::vector<std::string> m_HitPatternNames;
  std::vector<HitPattern> m_HitPatterns;
  std::vector<std::vector<std::function<bool (const BasicComptonEvent&)>>> m_ConditionsVector;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ComptonEventFilter_H */
