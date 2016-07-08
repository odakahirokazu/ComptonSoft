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

#ifndef ANLGEANT4_StandardPickUpData_H
#define ANLGEANT4_StandardPickUpData_H 1

#include "VPickUpData.hh"

namespace anlgeant4 {

class InitialInformation;

/**
 * @author Hirokazu Odaka
 * @date 2011-04-11
 * @date 2016-07-08 | setInitialTime()
 */
class StandardPickUpData : public VPickUpData
{
  DEFINE_ANL_MODULE(StandardPickUpData, 2.0);
public:
  StandardPickUpData();

  anl::ANLStatus mod_init();

  void EventAct_begin(const G4Event* aEvent);

protected:
  double getInitialTime() const;
  void setInitialTime(double v);

private:
  InitialInformation* m_InitialInfo;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_StandardPickUpData_H */
