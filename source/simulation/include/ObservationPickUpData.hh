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

#include "VAppendableUserActionAssembly.hh"
#include "ObservedParticle.hh"

namespace comptonsoft {


/**
 * PickUpData for observation from outside of the world.
 *
 * @author Hirokazu Odaka
 * @date 2017-06-20
 * @date 2017-06-29 | new design of VAppendableUserActionAssembly
 */
class ObservationPickUpData : public anlgeant4::VAppendableUserActionAssembly
{
  DEFINE_ANL_MODULE(ObservationPickUpData, 2.0);
public:
  ObservationPickUpData();
  ~ObservationPickUpData() = default;
  
  anl::ANLStatus mod_startup() override;

  void EventActionAtBeginning(const G4Event*) override;
  void TrackActionAtEnd(const G4Track* track) override;

  const std::vector<ObservedParticle_sptr>& getParticleVector() const 
  { return particleVector_; }

private:
  bool recordPrimaries_;
  std::vector<int> particleSelection_;
  std::vector<ObservedParticle_sptr> particleVector_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ObservationPickUpData_H */
