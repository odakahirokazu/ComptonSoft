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

#ifndef CSPhysicsList_H
#define CSPhysicsList_H 1

#include "G4VModularPhysicsList.hh"

namespace comptonsoft
{

class CSPhysicsOption
{
public:
  enum class EMModel { Livermore, LivermorePolarized, LivermoreCustomized };
  enum class HadronModel { BERT, BIC, INCLXX, BERT_HP, BIC_HP, BIC_AllHP };

public:
  CSPhysicsOption() = default;
  ~CSPhysicsOption() = default;

  void setEMPhysicsModel(EMModel v) { EMModel_ = v; }
  EMModel getEMPhysicsModel() const { return EMModel_; }

  void setHadronPhysicsModel(HadronModel v) { hadronModel_ = v; }
  HadronModel getHadronPhysicsModel() const { return hadronModel_; }

  void setHadronPhysicsHighPrecision(bool v=true) { hadronHP_ = v; }
  bool getHadronPhysicsHighPrecision() const { return hadronHP_; }

  void enableRadioactiveDecay(bool v=true) { RDEnabled_ = v; }
  bool isRadioactiveDecayEnabled() const { return RDEnabled_; }

private:
  EMModel EMModel_ = EMModel::Livermore;
  HadronModel hadronModel_ = HadronModel::BIC;
  bool hadronHP_ = false;
  bool RDEnabled_ = false;
};

/**
 * Geant4 physics list for X-ray and soft gamma-ray simulations.
 * This is a standard physics list of the ASTRO-H X-ray Observatory.
 *
 * @author Tamotsu Sato, Hirokazu Odaka
 * @date 2011-xx-xx
 * @date 2015-06-10 | Hirokazu Odaka | reviewd
 * @date 2016-01-28 | Hirokazu Odaka | enable parallel world
 */
class CSPhysicsList : public G4VModularPhysicsList
{
public:
  CSPhysicsList(CSPhysicsOption option);
  ~CSPhysicsList() = default;

  void ConstructProcess();
  void AddParallelWorldProcess();

  void enableParallelWorld(bool v=true) { parallelWorld_ = v; }
  void disableParallelWorld() { enableParallelWorld(false); }
  bool isParallelWorldEnabled() const { return parallelWorld_; }

private:
  bool parallelWorld_ = true;
};

} /* namespace comptonsoft */

#endif /* CSPhysicsList_H */
