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

namespace anlgeant4 {

InitialInformation::InitialInformation(bool stored, anlnext::BasicModule* mod)
  : stored_(stored),
    run_id_(-1)
{
  if (mod) {
    mod->add_alias("InitialInformation");
  }

  primary_vector_.resize(1);
}

void InitialInformation::initialize_run(int run_id, int num_events)
{
  set_run_id(run_id);
  primary_vector_.assign(num_events, PrimaryData());
  index_ = 0;
}

void InitialInformation::initialize_event(int event_id)
{
  primary_vector_[event_id].event_id = event_id;
}

} /* namespace anlgeant4 */
