/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#ifndef AHG4RadioactiveDecayConstructor_H
#define AHG4RadioactiveDecayConstructor_H 1

#include "globals.hh"
#include "G4VPhysicsConstructor.hh"


/**
 * Geant4 Physics constructor for radioactive dacay
 * @author Tamotsu Sato, Hirokazu Odaka
 * @date 2011-xx-xx
 */
class AHG4RadioactiveDecayConstructor : public G4VPhysicsConstructor
{
 public:
  AHG4RadioactiveDecayConstructor(G4int ver);
  virtual ~AHG4RadioactiveDecayConstructor();

  virtual void ConstructParticle();
  virtual void ConstructProcess();
};

#endif /* AHG4RadioactiveDecayConstructor_H */
