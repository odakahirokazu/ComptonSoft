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

#ifndef COMPTONSOFT_XrayEventSelection_H
#define COMPTONSOFT_XrayEventSelection_H 1

#include "XrayEventCollection.hh"
#include <functional>

namespace comptonsoft {

class XrayEvent;

class XrayEventSelectionCondition
{
public:
  void add_key(const std::string& key) { key_ = key; }
  std::string& Key() { return key_; }
  std::vector<std::function<bool (const XrayEvent&)>>& Conditions() { return conditions_; }
  void add_condition(const std::function<bool (const XrayEvent&)>& condition)
  { conditions_.push_back(condition); }

private:
  std::string key_;
  std::vector<std::function<bool (const XrayEvent&)>> conditions_;
};

/**
 * @author Hirokazu Odaka
 * @date 2019-10-30
 * @date 2020-12-08 | 2.0 | by Tsubasa Tamba
 */
class XrayEventSelection : public XrayEventCollection
{
  DEFINE_ANL_MODULE(XrayEventSelection, 2.0);
public:
  XrayEventSelection();
  ~XrayEventSelection();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_pre_initialize() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  void add_condition(const std::string& key, const std::vector<std::pair<int, int>>& ranges);
  void add_condition(const std::string& key, const std::vector<std::pair<double, double>>& ranges);

  template <typename T>
  void add_condition(const std::string& key, const std::vector<std::pair<T, T>>& ranges);

protected:
  virtual bool select(const XrayEvent_sptr& e) const;

private:
  anlnext::ANLStatus status_ = anlnext::AS_OK;
  std::vector<XrayEventSelectionCondition> conditionsVector_;
  const XrayEventCollection* collection_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_XrayEventSelection_H */
