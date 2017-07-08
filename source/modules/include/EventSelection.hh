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

#ifndef COMPTONSOFT_EventSelection_H
#define COMPTONSOFT_EventSelection_H 1

#include "VCSModule.hh"

#include <vector>
#include <memory>
#include <utility>

namespace comptonsoft {

class CSHitCollection;

/**
 * event selection
 * @author Hirokazu Odaka
 * @date 2008-08-28
 * @date 2014-11-25
 * @date 2015-10-10
 * @date 2016-09-20 | detectors off
 */
class EventSelection : public VCSModule
{
  DEFINE_ANL_MODULE(EventSelection, 2.2)
public:
  EventSelection();
  ~EventSelection() = default;

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;

private:
  bool m_DiscardTimeGroupZero;
  bool m_DiscardTimeGroupNonZero;
  bool m_OffEnabled;
  bool m_VetoEnabled;
  std::vector<std::pair<double, double>> m_FluoresenceRanges;
  CSHitCollection* m_HitCollection;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EventSelection_H */
