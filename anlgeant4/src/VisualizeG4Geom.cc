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

#include "VisualizeG4Geom.hh"

#include <sstream>
#include "AstroUnits.hh"

// #define G4VIS_USE 1
// #define G4VIS_USE_OPENGLX 1

#if ANLG4_USE_VIS
#define G4UI_USE 1
#define G4VIS_USE 1
#define G4VIS_USE_OPENGL 1
#define G4VIS_USE_OPENGLX 1
#endif

#if ANLG4_USE_VIS_QT
#define G4UI_BUILD_QT_SESSION 1
#define G4UI_USE_QT 1
#define G4VIS_BUILD_OPENGLQT_DRIVER 1
#define G4VIS_USE_OPENGLQT 1
#endif

//#undef G4VIS_USE_OPENGLX
//#undef G4VIS_USE

#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"


using namespace anlnext;

namespace anlgeant4
{

VisualizeG4Geom::VisualizeG4Geom()
  : m_UIManager(nullptr),
    m_Mode("OGL"),
    m_TargetPoint(0.0, 0.0, 0.0),
    m_ViewPoint(0.0, 1.0, 0.0),
    m_UpVector(0.0, 0.0, 1.0),
    m_Zoom(1.0),
    m_AuxiliaryEdge(true),
    m_MacroFile("")
{
}

VisualizeG4Geom::~VisualizeG4Geom() = default;

ANLStatus VisualizeG4Geom::mod_define()
{
  register_parameter(&m_Mode, "mode");
  register_parameter(&m_TargetPoint, "target_point", unit::cm, "cm");
  register_parameter(&m_ViewPoint, "view_point");
  register_parameter(&m_UpVector, "up_direction");
  register_parameter(&m_Zoom, "zoom");
  register_parameter(&m_AuxiliaryEdge, "auxiliary_edge");
  register_parameter(&m_MacroFile, "macro_file");
  
  return AS_OK;
}

ANLStatus VisualizeG4Geom::mod_initialize()
{
  m_VisManager.reset(new G4VisExecutive);
  m_UIManager = G4UImanager::GetUIpointer();
  m_VisManager->Initialize();

  static char* argv[1];
  static char name[16] = "ANLGeant4";
  argv[0] = name;
  m_UIExecutive.reset(new G4UIExecutive(1, argv));

  if (m_MacroFile=="") {
    applyDefaultCommands();
  }
  else {
    std::ostringstream cmd;
    cmd << "/control/execute " << m_MacroFile;
    m_UIManager->ApplyCommand(cmd.str());
  }

  return AS_OK;
}

ANLStatus VisualizeG4Geom::mod_end_run()
{
  if (m_Mode.find("OGL") != std::string::npos) {
    m_UIExecutive->SessionStart();
  }
  
  return AS_OK;
}

ANLStatus VisualizeG4Geom::mod_finalize()
{
  m_UIExecutive.reset();
  m_VisManager.reset();

  return AS_OK;
}

void VisualizeG4Geom::applyDefaultCommands()
{
  m_UIManager->ApplyCommand("/control/verbose 2");
  
  std::ostringstream cmd;
  cmd.str("");
  cmd << "/vis/open " << m_Mode << " 600x600-0+0";
  m_UIManager->ApplyCommand(cmd.str());
  cmd.str("");

  m_UIManager->ApplyCommand("/vis/viewer/set/autoRefresh false");
  m_UIManager->ApplyCommand("/vis/verbose errors");
  m_UIManager->ApplyCommand("/vis/drawVolume worlds");

  cmd.str("");
  cmd << "/vis/viewer/set/targetPoint "
      << m_TargetPoint.x()/unit::cm << " "
      << m_TargetPoint.y()/unit::cm << " "
      << m_TargetPoint.z()/unit::cm << " cm";
  m_UIManager->ApplyCommand(cmd.str());
  cmd.str("");

  cmd.str("");
  cmd << "/vis/viewer/set/viewpointVector "
      << m_ViewPoint.x() << " "
      << m_ViewPoint.y() << " "
      << m_ViewPoint.z();
  m_UIManager->ApplyCommand(cmd.str());
  cmd.str("");
  
  cmd.str("");
  cmd << "/vis/viewer/set/upVector "
      << m_UpVector.x() << " "
      << m_UpVector.y() << " "
      << m_UpVector.z();
  m_UIManager->ApplyCommand(cmd.str());
  cmd.str("");
  
  cmd << "/vis/viewer/zoom "<< m_Zoom;
  m_UIManager->ApplyCommand(cmd.str());
  cmd.str("");

  cmd << "/vis/viewer/set/auxiliaryEdge "<< m_AuxiliaryEdge;
  m_UIManager->ApplyCommand(cmd.str());
  cmd.str("");
  
  m_UIManager->ApplyCommand("/vis/viewer/set/style wireframe");
  m_UIManager->ApplyCommand("/vis/scene/add/trajectories smooth");
  m_UIManager->ApplyCommand("/vis/scene/add/hits");
  
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/create/drawByParticleID");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set e- red"); 
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set e+ blue");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set proton cyan");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set gamma green");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set neutron yellow");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi+ magenta");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi- magenta");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi0 magenta");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/default/setDrawStepPts true");
  m_UIManager->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/default/setStepPtsSize 2");
  
  m_UIManager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
  m_UIManager->ApplyCommand("/vis/viewer/set/autoRefresh true");
  m_UIManager->ApplyCommand("/vis/verbose warnings");
  
  m_UIManager->ApplyCommand("/vis/viewer/flush");

  if (m_UIExecutive->IsGUI()) {
    m_UIManager->ApplyCommand("/gui/addMenu viewer Viewer");
    m_UIManager->ApplyCommand("/gui/addButton viewer \"Set style surface\" \"/vis/viewer/set/style s\"");
    m_UIManager->ApplyCommand("/gui/addButton viewer \"Set style wireframe\" \"/vis/viewer/set/style w\"");
  }
}

} /* namespace anlgeant4 */
