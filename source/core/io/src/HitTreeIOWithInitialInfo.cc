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

#include "HitTreeIOWithInitialInfo.hh"

namespace comptonsoft
{

HitTreeIOWithInitialInfo::~HitTreeIOWithInitialInfo() = default;

void HitTreeIOWithInitialInfo::set_tree(TTree* tree)
{
  HitTreeIO::set_tree(tree);
  InitialInfoTreeIO::set_tree(tree);
}

void HitTreeIOWithInitialInfo::define_branches()
{
  HitTreeIO::define_branches();
  InitialInfoTreeIO::define_branches();
}

void HitTreeIOWithInitialInfo::set_branch_addresses()
{
  HitTreeIO::set_branch_addresses();
  InitialInfoTreeIO::set_branch_addresses();
}

} /* namespace comptonsoft */
