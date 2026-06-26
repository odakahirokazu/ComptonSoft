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

#ifndef COMPTONSOFT_ObservationPickUpData_H
#define COMPTONSOFT_ObservationPickUpData_H 1

#include "VUserActionAssembly.hh"
#include "ObservedParticle.hh"

namespace comptonsoft {


/**
 * PickUpData for observation from outside of the world.
 *
 * @author Hirokazu Odaka
 * @date 2017-06-20
 * @date 2017-06-29 | new design of VAppendableUserActionAssembly
 * @date 2026-04-15 | use VUserActionAssembly
 */
class ObservationPickUpData : public anlgeant4::VUserActionAssembly
{
  DEFINE_ANL_MODULE(ObservationPickUpData, 3.0);
  ENABLE_PARALLEL_RUN();
public:
  ObservationPickUpData();
  ~ObservationPickUpData() = default;

  anlnext::ANLStatus mod_define() override;

  void event_action_at_beginning(const G4Event*) override;
  void track_action_at_end(const G4Track* track) override;

  const std::vector<ObservedParticle_sptr>& getParticleVector() const
  { return particleVector_; }

private:
  bool recordPrimaries_;
  std::vector<int> particleSelection_;
  std::vector<ObservedParticle_sptr> particleVector_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ObservationPickUpData_H */
