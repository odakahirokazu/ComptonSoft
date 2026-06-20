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

#include <string>
#include <memory>
#include "PrimarySetting.hh"

class G4Event;
class G4ParticleDefinition;
class G4ParticleGun;

namespace anlgeant4 {

class BasicPrimaryGen;

/**
 * Primary generator action
 *
 * @author Hirokazu Odaka
 * @date 2010-xx-xx
 * @date 2017-06-27 | tweaks
 * @date 2026-06-20 | Geant4-MT
 */
class BasicPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  BasicPrimaryGeneratorAction();
  virtual ~BasicPrimaryGeneratorAction();

  void GeneratePrimaries(G4Event* event) override;

  void RegisterSampler(anlgeant4::BasicPrimaryGen* sampler) { sampler_ = sampler; }

protected:
  void SetDefinition(G4ParticleDefinition* definition);
  void SetPrimarySetting(const PrimarySetting& primary);

private:
  std::unique_ptr<G4ParticleGun> particle_gun_;
  anlgeant4::BasicPrimaryGen* sampler_ = nullptr;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_BasicPrimaryGeneratorAction_H */
