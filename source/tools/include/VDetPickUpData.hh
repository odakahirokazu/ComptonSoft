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

#ifndef COMPTONSOFT_VDetPickUpData_H
#define COMPTONSOFT_VDetPickUpData_H 1

#include <vector>
#include <string>

class G4Event;
class G4Track;
class G4Step;

namespace comptonsoft {

/**
 * a virtual class of a pick up data.
 */

class VDetPickUpData
{
public:
  VDetPickUpData()
  {
  }
  virtual ~VDetPickUpData()
  {
  }

  virtual void EventAct_begin(const G4Event* )
  {
  }

  virtual void EventAct_end(const G4Event* )
  {
  }

  virtual void TrackAct_begin(const G4Track* )
  {
  }

  virtual void TrackAct_end(const G4Track* )
  {
  }

  virtual void StepAct(const G4Step*, G4Track*)
  {
  }
  
protected:
  std::vector <std::string> identifier;

public:
  std::vector <std::string> GetIdentifier(){return identifier;}

};

}

#endif /* COMPTONSOFT_VDetPickUpData_H */
