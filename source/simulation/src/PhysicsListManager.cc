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

#include "PhysicsListManager.hh"
#include "G4SystemOfUnits.hh"

using namespace anl;

namespace comptonsoft
{

PhysicsListManager::PhysicsListManager()
  : m_EMPolarization(false),
    m_EMCustomized(false),
    m_HadronHP(false),
    m_HadronModel("BIC"),
    m_RDEnabled(false),
    m_DefaultCut(0.0001*cm)
{
  add_alias(module_name());
}

ANLStatus PhysicsListManager::mod_startup()
{
  anlgeant4::VANLPhysicsList::mod_startup();
  register_parameter(&m_DefaultCut, "cut_value", cm, "cm");
  register_parameter(&m_EMPolarization, "polarization");
  register_parameter(&m_EMCustomized, "customized_em");
  register_parameter(&m_HadronHP, "hadron_hp");
  register_parameter(&m_HadronModel, "hadron_model");
  register_parameter(&m_RDEnabled, "radioactive_decay");
  
  return AS_OK;
}

ANLStatus PhysicsListManager::mod_init()
{
  CSPhysicsOption option;

  // EM physics model if (m_EMPolarization) {
  if (m_EMPolarization) {
    m_PhysicsOption.setEMPhysicsModel(CSPhysicsOption::EMModel::LivermorePolarized);
    if (m_EMCustomized) {
      std::cout << "PhysicsListManager: \n"
                << "Customized version of LivermorePolarized is not prepared.\n"
                << std::endl;
      return AS_QUIT_ERR;
    }
  }
  else if (m_EMCustomized) {
    m_PhysicsOption.setEMPhysicsModel(CSPhysicsOption::EMModel::LivermoreCustomized);
  }
  else {
    m_PhysicsOption.setEMPhysicsModel(CSPhysicsOption::EMModel::Livermore);
  }

  // hardron high-precision model
  m_PhysicsOption.setHadronPhysicsHighPrecision(m_HadronHP);

  // hardron inelastic interaction model
  if (m_HadronModel=="BERT") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BERT);
  }
  else if (m_HadronModel=="BIC") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BIC);
  }
  else if (m_HadronModel=="INCLXX" || m_HadronModel=="INCL++") {
    m_PhysicsOption.setHadronPhysicsModel(CSPhysicsOption::HadronModel::INCLXX);
  }
  else {
    std::cout << "PhysicsListManager: \n"
              << "Unknown hadron model is given. ===> " << m_HadronModel
              << std::endl;
    return AS_QUIT_ERR;
  }

  // radioactive decay
  m_PhysicsOption.enableRadioactiveDecay(m_RDEnabled);

  return AS_OK;
}

G4VUserPhysicsList* PhysicsListManager::create()
{
  G4VUserPhysicsList* physicsList = new CSPhysicsList(m_PhysicsOption);
  physicsList->SetDefaultCutValue(m_DefaultCut);
  return physicsList;
}

} /* namespace comptonsoft */
