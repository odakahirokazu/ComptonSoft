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
#include "AstroUnits.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BIC_HP.hh"
#include "FTFP_BERT.hh"
#include "FTFP_BERT_HP.hh"
#include "FTFP_INCLXX.hh"
#include "FTFP_INCLXX_HP.hh"
#include "Shielding.hh"
#include "G4NuclearLevelData.hh"
#include "PhysicsListWithRadioactiveDecay.hh"

using namespace anlnext;

namespace comptonsoft
{

PhysicsListManager::PhysicsListManager()
  : physics_list_name_("CSPhysicsList"),
    em_polarization_(false),
    em_customized_(false),
    em_option_fluo_(true),
    em_option_auger_(false),
    em_option_pipx_(false),
    electron_range_ratio_(0.2),
    electron_final_range_(100.0*CLHEP::um),
    hadron_hp_(false),
    hadron_model_("BIC"),
    nuclear_lifetime_threshold_(1.0e-12*CLHEP::second),
    radioactive_decay_enabled_(false),
    parallel_world_enabled_(true),
    default_cut_(0.0001*CLHEP::cm)
{
  add_alias(module_name());
}

ANLStatus PhysicsListManager::mod_define()
{
  anlgeant4::VANLPhysicsList::mod_define();
  define_parameter("cut_value", &mod_class::default_cut_, CLHEP::cm, "cm");
  define_parameter("physics_list", &mod_class::physics_list_name_);
  define_parameter("polarization", &mod_class::em_polarization_);
  define_parameter("customized_em", &mod_class::em_customized_);
  define_parameter("em_option_fluo", &mod_class::em_option_fluo_);
  define_parameter("em_option_auger", &mod_class::em_option_auger_);
  define_parameter("em_option_pixe", &mod_class::em_option_pipx_);
  define_parameter("electron_range_ratio", &mod_class::electron_range_ratio_);
  define_parameter("electron_final_range", &mod_class::electron_final_range_, CLHEP::cm, "cm");
  define_parameter("hadron_hp", &mod_class::hadron_hp_);
  define_parameter("hadron_model", &mod_class::hadron_model_);
  define_parameter("nuclear_lifetime_threshold", &mod_class::nuclear_lifetime_threshold_, CLHEP::second, "s");
  define_parameter("radioactive_decay", &mod_class::radioactive_decay_enabled_);
  define_parameter("parallel_world", &mod_class::parallel_world_enabled_);

  return AS_OK;
}

ANLStatus PhysicsListManager::mod_pre_initialize()
{
  if (physics_list_name_ == "CSPhysicsList") {
    if (!em_customized_) {
      hide_parameter("em_option_fluo");
      hide_parameter("em_option_auger");
      hide_parameter("em_option_pixe");
      hide_parameter("electron_range_ratio");
      hide_parameter("electron_final_range");
    }
  }
  else {
    hide_parameter("polarization");
    hide_parameter("customized_em");
    hide_parameter("em_option_fluo");
    hide_parameter("em_option_auger");
    hide_parameter("em_option_pixe");
    hide_parameter("electron_range_ratio");
    hide_parameter("electron_final_range");
    hide_parameter("hadron_hp");
    hide_parameter("hadron_model");
    hide_parameter("radioactive_decay");
    hide_parameter("parallel_world");
  }

  return AS_OK;
}

ANLStatus PhysicsListManager::mod_initialize()
{
  CSPhysicsOption option;

  // EM physics model
  if (em_customized_) {
    if (em_polarization_) {
      physics_option_.setEMPhysicsModel(CSPhysicsOption::EMModel::CustomizedLivermorePolarized);
    }
    else {
      physics_option_.setEMPhysicsModel(CSPhysicsOption::EMModel::CustomizedLivermore);
    }
    physics_option_.setEMOptions(em_option_fluo_, em_option_auger_, em_option_pipx_);
    physics_option_.setElectronRangeParameters(electron_range_ratio_, electron_final_range_);
  }
  else {
    if (em_polarization_) {
      physics_option_.setEMPhysicsModel(CSPhysicsOption::EMModel::LivermorePolarized);
    }
    else {
      physics_option_.setEMPhysicsModel(CSPhysicsOption::EMModel::Livermore);
    }
  }

  // hardron high-precision model
  physics_option_.setHadronPhysicsHighPrecision(hadron_hp_);

  // hardron inelastic interaction model
  if (hadron_model_=="BERT") {
    physics_option_.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BERT);
  }
  else if (hadron_model_=="BIC") {
    physics_option_.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BIC);
  }
  else if (hadron_model_=="INCLXX" || hadron_model_=="INCL++") {
    physics_option_.setHadronPhysicsModel(CSPhysicsOption::HadronModel::INCLXX);
  }
  else if (hadron_model_=="BERT_HP") {
    physics_option_.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BERT_HP);
  }
  else if (hadron_model_=="BIC_HP") {
    physics_option_.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BIC_HP);
  }
  else if (hadron_model_=="BIC_AllHP") {
    physics_option_.setHadronPhysicsModel(CSPhysicsOption::HadronModel::BIC_AllHP);
  }
  else {
    std::cout << "PhysicsListManager: \n"
              << "Unknown hadron model is given. ===> " << hadron_model_
              << std::endl;
    return AS_QUIT_ERROR;
  }

  // radioactive decay
  physics_option_.enableRadioactiveDecay(radioactive_decay_enabled_);

  // set geant4 parameters
  G4NuclearLevelData::GetInstance()->GetParameters()->SetMaxLifeTime(nuclear_lifetime_threshold_);

  return AS_OK;
}

G4VUserPhysicsList* PhysicsListManager::create()
{
  G4VUserPhysicsList* physicsList(nullptr);

  if (physics_list_name_ == "CSPhysicsList") {
    CSPhysicsList* pl = new CSPhysicsList(physics_option_);
    pl->enableParallelWorld(parallel_world_enabled_);
    physicsList = pl;
  }
  else if (physics_list_name_ == "QGSP_BIC") {
    physicsList = new QGSP_BIC;
  }
  else if (physics_list_name_ == "QGSP_BIC_HP") {
    physicsList = new QGSP_BIC_HP;
  }
  else if (physics_list_name_ == "QGSP_BIC_HP_RD") {
    physicsList = new PhysicsListWithRadioactiveDecay<QGSP_BIC_HP>;
  }
  else if (physics_list_name_ == "FTFP_BERT") {
    physicsList = new FTFP_BERT;
  }
  else if (physics_list_name_ == "FTFP_BERT_HP") {
    physicsList = new FTFP_BERT_HP;
  }
  else if (physics_list_name_ == "FTFP_BERT_HP_RD") {
    physicsList = new PhysicsListWithRadioactiveDecay<FTFP_BERT_HP>;
  }
  else if (physics_list_name_ == "FTFP_INCLXX") {
    physicsList = new FTFP_INCLXX;
  }
  else if (physics_list_name_ == "FTFP_INCLXX_HP") {
    physicsList = new FTFP_INCLXX_HP;
  }
  else if (physics_list_name_ == "FTFP_INCLXX_HP_RD") {
    physicsList = new PhysicsListWithRadioactiveDecay<FTFP_INCLXX_HP>;
  }
  else if (physics_list_name_ == "Shielding") {
    physicsList = new Shielding;
  }
  else {
    std::cout << "PhysicsListManager: unknown physics list is given: "
              << physics_list_name_
              << std::endl;
    return physicsList;
  }

  physicsList->SetDefaultCutValue(default_cut_);

  return physicsList;
}

} /* namespace comptonsoft */
