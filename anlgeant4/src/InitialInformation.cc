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

#include "InitialInformation.hh"
#include <anlnext/BasicModule.hh>

anlgeant4::
InitialInformation::InitialInformation(bool stored, anlnext::BasicModule* mod)
  : stored_(stored), weight_stored_(stored),
    energy_(0.0), direction_(0.0, 0.0, -1.0),
    time_(0.0), position_(0.0, 0.0, 0.0),
    polarization_(0.0, 0.0, 0.0),
    event_id_(-1), weight_(1.0)
{
  if (mod) {
    mod->add_alias("InitialInformation");
  }
}
