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

class ComptonEventSelectionCondition
{
public:
  ComptonEventSelectionCondition() = default;
  ~ComptonEventSelectionCondition() = default;

  void add_evs_key(const std::string& key)
  { evsKeys_.push_back(key); }
  void add_hit_pattern(const std::string& name)
  { hitPatterns_.push_back(name); }
  void add_condition(const std::function<bool (const BasicComptonEvent&)>& condition)
  { conditions_.push_back(condition); }

  std::vector<std::string>& get_evs_keys()
  { return evsKeys_; }
  std::vector<std::string>& get_hit_patterns()
  { return hitPatterns_; }
  std::vector<std::function<bool (const BasicComptonEvent&)>>& get_conditions()
  { return conditions_; }

private:
  std::vector<std::string> evsKeys_;
  std::vector<std::string> hitPatterns_;
  std::vector<std::function<bool (const BasicComptonEvent&)>> conditions_;
};

/**
 *
 * @author Hirokazu Odaka
 * @date 2011-xx-xx
 * @date 2014-11-26
 * @date 2015-10-10 | derived from VCSModule
 * @date 2017-02-06 | 4.0 | do not determine hit patterns in this module.
 * @date 2017-02-15 | 4.1 | can specify hit pattern in the condition phase
 */
class ComptonEventFilter : public VCSModule
{
  DEFINE_ANL_MODULE(ComptonEventFilter, 4.1);
public:
  ComptonEventFilter();
  ~ComptonEventFilter() = default;

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;

  void define_condition();
  void add_hit_pattern(const std::string& name);
  void add_evs_key(const std::string& key);
  void add_condition(const std::string& type,
                     double minValue,
                     double maxValue);
    
private:
  EventReconstruction* m_EventReconstruction;
  std::vector<ComptonEventSelectionCondition> m_ConditionsVector;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ComptonEventFilter_H */
