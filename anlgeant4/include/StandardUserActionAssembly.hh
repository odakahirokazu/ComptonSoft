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

#ifndef ANLGEANT4_StandardUserActionAssembly_H
#define ANLGEANT4_StandardUserActionAssembly_H 1

#include "VUserActionAssembly.hh"


namespace anlgeant4 {

class VEventStore;

/**
 * @author Hirokazu Odaka
 * @date 2011-04-11
 * @date 2016-07-08 | setInitialTime()
 * @date 2017-06-28 | Hirokazu Odaka | redesign, rename class and methods
 * @date 2026-04-15
 * @date 2026-05-26
 */
class StandardUserActionAssembly : public VUserActionAssembly
{
  DEFINE_ANL_MODULE(StandardUserActionAssembly, 6.0);
public:
  StandardUserActionAssembly();

  anlnext::ANLStatus mod_initialize() override;

  void RunActionAtBeginning(const G4Run* run) override;
  void RunActionAtEnd(const G4Run*) override;

  void EventActionAtBeginning(const G4Event* event) override;
  void EventActionAtEnd(const G4Event*) override;

protected:
  double getInitialTime() const;
  void setInitialTime(size_t event_index, double v);

private:
  VEventStore* event_store_ = nullptr;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_StandardUserActionAssembly_H */
