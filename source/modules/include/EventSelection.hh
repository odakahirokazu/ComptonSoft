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
 */
class EventSelection : public VCSModule
{
  DEFINE_ANL_MODULE(EventSelection, 2.1)
public:
  EventSelection();
  ~EventSelection() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();

private:
  bool m_VetoEnabled;
  bool m_DiscardTimeGroupZero;
  bool m_DiscardTimeGroupNonZero;
  CSHitCollection* m_HitCollection;
  std::vector<std::pair<double, double>> m_FluoresenceRanges;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EventSelection_H */
