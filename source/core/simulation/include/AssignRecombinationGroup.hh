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

#ifndef COMPTONSOFT_AssignRecombinationGroup_H
#define COMPTONSOFT_AssignRecombinationGroup_H 1

#include "VUserActionAssembly.hh"

namespace comptonsoft {

/**
 * Assign a recombination group to each track and register it to RecombinationGroupTable.
 * A primary electron (or an electron created by a photon) and all of its charged descendants form one group,
 * so that the recombination is calculated from the total energy of the group.
 *
 * @author Shota Arai
 * @date 2026-10-27
 */
class AssignRecombinationGroup : public anlgeant4::VUserActionAssembly
{
  DEFINE_ANL_MODULE(AssignRecombinationGroup, 1.0);
  ENABLE_PARALLEL_RUN();
public:
  AssignRecombinationGroup();
  ~AssignRecombinationGroup() = default;

  void event_action_at_beginning(const G4Event* event) override;
  void track_action_at_beginning(const G4Track* track) override;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AssignRecombinationGroup_H */
