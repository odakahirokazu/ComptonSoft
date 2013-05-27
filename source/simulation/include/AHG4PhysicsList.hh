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

#ifndef AHG4PhysicsList_H
#define AHG4PhysicsList_H 1

#include "G4VModularPhysicsList.hh"


/**
 * Geant4 physics list for X-ray and soft gamma-ray simulations.
 * This is a standard physics list of the ASTRO-H X-ray Observatory.
 *
 * @author Tamotsu Sato, Hirokazu Odaka
 * @date 2011-xx-xx
 */
class AHG4PhysicsList : public G4VModularPhysicsList
{
public:
  AHG4PhysicsList(G4String option="");
  ~AHG4PhysicsList() {}

  virtual void ConstructProcess();
  virtual void SetCuts();
  
private:
  void AddParallelWorldProcess();
};

#endif /* AHG4PhysicsList_H */
