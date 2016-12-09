/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Authors of G4EmLivermorePhysics, Hirokazu Odaka    *
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

#ifndef AHG4EmLivermorePhysics_H
#define AHG4EmLivermorePhysics_H 1

#include "G4VPhysicsConstructor.hh"

/**
 * ASTRO-H standard EM physics.
 * This class is based on G4EmLivermorePhysics.
 * Customizing of auger electrons, PIXE, and electron final ranges is allowed.
 *
 * @author Hirokazu Odaka
 * @date 2011-xx-xx | Hirokazu Odaka
 */
class AHG4EmLivermorePhysics : public G4VPhysicsConstructor
{
public:
  AHG4EmLivermorePhysics(G4int ver=1);

  // obsolete
  AHG4EmLivermorePhysics(G4int ver, const G4String& name);

  virtual ~AHG4EmLivermorePhysics();

  virtual void ConstructParticle();
  virtual void ConstructProcess();

  void SetElectronFinalRange(G4double v) { m_ElectronFinalRange = v; }
  void ActivateAuger(G4bool v) { m_AugerOn = v; }
  void ActivatePIXE(G4bool v) { m_PIXEOn = v; }
  
private:
  G4int verbose;

  G4double m_ElectronFinalRange;
  G4bool m_AugerOn;
  G4bool m_PIXEOn;
};

#endif /* AHG4EmLivermorePhysics_H */
