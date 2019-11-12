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

#include "XrayEventSelection.hh"

using namespace anlnext;

namespace comptonsoft
{

XrayEventSelection::XrayEventSelection()
{
}

XrayEventSelection::~XrayEventSelection() = default;

ANLStatus XrayEventSelection::mod_define()
{
  define_parameter("ix_min", &mod_class::ixMin_);
  define_parameter("ix_max", &mod_class::ixMax_);
  define_parameter("iy_min", &mod_class::iyMin_);
  define_parameter("iy_max", &mod_class::iyMax_);
  define_parameter("sumPH_min", &mod_class::sumPHMin_);
  define_parameter("sumPH_max", &mod_class::sumPHMax_);
  define_parameter("rank_min", &mod_class::rankMin_);
  define_parameter("rank_max", &mod_class::rankMax_);
  define_parameter("weight_min", &mod_class::weightMin_);
  define_parameter("weight_max", &mod_class::weightMax_);

  return AS_OK;
}

ANLStatus XrayEventSelection::mod_initialize()
{
  get_module("XrayEventCollection", &collection_);

  return AS_OK;
}

ANLStatus XrayEventSelection::mod_analyze()
{
  initializeEvent();

  const XrayEventContainer& events = collection_->getEvents();
  for (const XrayEvent_sptr& e: events) {
    if (select(e)) {
      insertEvent(e);
    }
  }

  return AS_OK;
}

bool XrayEventSelection::select(const XrayEvent_sptr& e) const
{
  const double sumPH = e->SumPH();
  const int rank = e->Rank();
  const int weight = e->Weight();
  const int ix = e->X();
  const int iy = e->Y();

  return (sumPHMin_<=sumPH && sumPH<=sumPHMax_ &&
          rankMin_<=rank && rank<=rankMax_ &&
          weightMin_<=weight && weight<=weightMax_ &&
          ixMin_<=ix && ix<=ixMax_ &&
          iyMin_<=iy && iy<=iyMax_);
}

} /* namespace comptonsoft */
