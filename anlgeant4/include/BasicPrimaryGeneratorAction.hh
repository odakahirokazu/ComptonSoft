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

#ifndef ANLGEANT4_BasicPrimaryGeneratorAction_H
#define ANLGEANT4_BasicPrimaryGeneratorAction_H 1

#include "G4VUserPrimaryGeneratorAction.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4ParticleGun;
class G4Event;
class G4ParticleDefinition;

namespace anlgeant4 {

/**
 * Primary generator action
 *
 * @author Hirokazu Odaka
 * @date 2010-xx-xx
 */
class BasicPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  BasicPrimaryGeneratorAction();
  explicit BasicPrimaryGeneratorAction(G4ParticleDefinition* definition);
  explicit BasicPrimaryGeneratorAction(G4String particle_name);  
  virtual ~BasicPrimaryGeneratorAction();
  
  virtual void GeneratePrimaries(G4Event* anEvent);

  void SetDefinition(G4ParticleDefinition* definition);
  
  void Set(G4double time0,
           const G4ThreeVector& position,
           G4double energy,
           const G4ThreeVector& direction)
  {
    m_Time = time0;
    m_Position = position;
    m_Energy = energy;
    m_Direction = direction;
    m_Polarization.set(0., 0., 0.);
  }

  void Set(G4double time0,
           const G4ThreeVector& position,
           G4double energy,
           const G4ThreeVector& direction,
           const G4ThreeVector& polarization)
  {
    m_Time = time0;
    m_Position = position;
    m_Energy = energy;
    m_Direction = direction;
    m_Polarization = polarization;
  }

  void Set(const G4ThreeVector& position,
           G4double energy,
           const G4ThreeVector& direction)
  {
    m_Time = 0.0;
    m_Position = position;
    m_Energy = energy;
    m_Direction = direction;
    m_Polarization.set(0., 0., 0.);
  }

  void Set(const G4ThreeVector& position,
           G4double energy,
           const G4ThreeVector& direction,
           const G4ThreeVector& polarization)
  {
    m_Time = 0.0;
    m_Position = position;
    m_Energy = energy;
    m_Direction = direction;
    m_Polarization = polarization;
  }

private:
  G4ParticleGun* m_ParticleGun;
  G4double m_Time;
  G4ThreeVector m_Position;
  G4double m_Energy;
  G4ThreeVector m_Direction;
  G4ThreeVector m_Polarization;
};

}

#endif /* ANLGEANT4_BasicPrimaryGeneratorAction_H */
