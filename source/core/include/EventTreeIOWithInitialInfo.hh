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

#ifndef COMPTONSOFT_EventTreeIOWithInitialInfo_H
#define COMPTONSOFT_EventTreeIOWithInitialInfo_H 1

#include "EventTreeIO.hh"
#include "InitialInfoTreeIO.hh"

namespace comptonsoft {

/**
 * 
 * @author Hirokazu Odaka
 * @date 2015-10-11
 */
class EventTreeIOWithInitialInfo : public EventTreeIO, public InitialInfoTreeIO
{
public:
  EventTreeIOWithInitialInfo() = default;
  ~EventTreeIOWithInitialInfo();

  void setTree(TTree* tree);
  void defineBranches();
  void setBranchAddresses();
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EventTreeIOWithInitialInfo_H */
