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

#include "ObservationTreeIOWithInitialInfo.hh"

namespace comptonsoft
{

ObservationTreeIOWithInitialInfo::~ObservationTreeIOWithInitialInfo() = default;

void ObservationTreeIOWithInitialInfo::set_tree(TTree* tree)
{
  ObservationTreeIO::set_tree(tree);
  InitialInfoTreeIO::set_tree(tree);
}

void ObservationTreeIOWithInitialInfo::define_branches()
{
  ObservationTreeIO::define_branches();
  InitialInfoTreeIO::define_branches();
}

void ObservationTreeIOWithInitialInfo::set_branch_addresses()
{
  ObservationTreeIO::set_branch_addresses();
  InitialInfoTreeIO::set_branch_addresses();
}

} /* namespace comptonsoft */
