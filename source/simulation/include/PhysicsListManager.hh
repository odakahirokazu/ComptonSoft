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

#ifndef COMPTONSOFT_PhysicsListManager_H
#define COMPTONSOFT_PhysicsListManager_H 1

#include <string>
#include "VANLPhysicsList.hh"
#include "CSPhysicsList.hh"

namespace comptonsoft
{

/**
 * ANL Geant4 physics list module for X-ray and soft gamma-ray simulations.
 * This is a standard physics list module for ASTRO-H.
 *
 * @author Hirokazu Odaka
 * @date 2011-xx-xx
 * @date 2012-05-30
 * @date 2015-06-10 | version 2.0
 * @date 2016-12-16 | version 2.1 | introduce parameter nuclear_lifetime_threshold.
 */
class PhysicsListManager : public anlgeant4::VANLPhysicsList
{
  DEFINE_ANL_MODULE(PhysicsListManager, 2.1);
public:
  PhysicsListManager();
  ~PhysicsListManager() = default;
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_prepare();
  anl::ANLStatus mod_init(); 

  G4VUserPhysicsList* create();

private:
  std::string m_PhysicsListName;
  bool m_EMPolarization;
  bool m_EMCustomized;
  bool m_HadronHP;
  std::string m_HadronModel;
  double m_NuclearLifeTimeThreshold;
  bool m_RDEnabled;
  bool m_ParallelWorldEnabled;
  double m_DefaultCut;
  CSPhysicsOption m_PhysicsOption;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PhysicsListManager_H */
