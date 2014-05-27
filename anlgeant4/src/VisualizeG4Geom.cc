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


using namespace anl;
using namespace anlgeant4;
using CLHEP::cm;


VisualizeG4Geom::VisualizeG4Geom()
  : m_VisManager(0), m_UIManager(0), m_UIExecutive(0),
    m_Mode("OGL"),
    m_TargetPoint(0.0, 0.0, 0.0),
    m_ViewPoint(0.0, 1.0, 0.0), m_UpVector(0.0, 0.0, 1.0),
    m_Zoom(1.0),
    m_AuxiliaryEdge(true)
{
}


VisualizeG4Geom::~VisualizeG4Geom()
{
  if (m_UIExecutive) delete m_UIExecutive;
  if (m_VisManager) delete m_VisManager;
}


ANLStatus VisualizeG4Geom::mod_startup()
{
  register_parameter(&m_Mode, "Mode");
  register_parameter(&m_TargetPoint, "Target point", cm, "cm");
  register_parameter(&m_ViewPoint, "View point");
  register_parameter(&m_UpVector, "Up direction");
  register_parameter(&m_Zoom, "Zoom");
  register_parameter(&m_AuxiliaryEdge, "Auxiliary edge");
  
  return AS_OK;
}


ANLStatus VisualizeG4Geom::mod_init()
{
  m_VisManager = new G4VisExecutive;
  m_UIManager = G4UImanager::GetUIpointer();
  m_VisManager->Initialize();

  char* argv[1];
  char name[16] = "ANLGEANT4";
  argv[0] = name;
  m_UIExecutive = new G4UIExecutive(1, argv);

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
      << m_TargetPoint.x()/cm << " "
      << m_TargetPoint.y()/cm << " "
      << m_TargetPoint.z()/cm << " cm";
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
  
  // m_UIManager->ApplyCommand("/vis/viewer/flush");

  if (m_UIExecutive->IsGUI()) {
    m_UIManager->ApplyCommand("/gui/addMenu geant4 Geant4");
    m_UIManager->ApplyCommand("/gui/addButton geant4 Exit \"exit\"");
    
    m_UIManager->ApplyCommand("/gui/addMenu run Run");
    m_UIManager->ApplyCommand("/gui/addButton run \"beamOn    1\" \"/run/beamOn 1\"");
    m_UIManager->ApplyCommand("/gui/addButton run \"beamOn   10\" \"/run/beamOn 10\"");
    m_UIManager->ApplyCommand("/gui/addButton run \"beamOn  100\" \"/run/beamOn 100\"");
    m_UIManager->ApplyCommand("/gui/addButton run \"beamOn 1000\" \"/run/beamOn 1000\"");
    
    m_UIManager->ApplyCommand("/gui/addMenu viewer Viewer");
    m_UIManager->ApplyCommand("/gui/addButton viewer \"Set style surface\" \"/vis/viewer/set/style surface\"");
    m_UIManager->ApplyCommand("/gui/addButton viewer \"Set style wireframe\" \"/vis/viewer/set/style wire\"");
    m_UIManager->ApplyCommand("/gui/addButton viewer \"Refresh viewer\" \"/vis/viewer/refresh\"");
    m_UIManager->ApplyCommand("/gui/addButton viewer \"Update viewer (interaction or end-of-file)\" \"/vis/viewer/update\"");
    m_UIManager->ApplyCommand("/gui/addButton viewer \"Flush viewer (= refresh + update)\" \"/vis/viewer/flush\"");
    m_UIManager->ApplyCommand("/gui/addButton viewer \"Update scene\" \"/vis/scene/notifyHandlers\"");
  }
  
  return AS_OK;
}


ANLStatus VisualizeG4Geom::mod_endrun()
{
  if (m_Mode.find("OGL") != std::string::npos) {
    m_UIExecutive->SessionStart();
  }
  
  return AS_OK;
}


ANLStatus VisualizeG4Geom::mod_exit()
{
  delete m_VisManager;
  m_VisManager = 0;
  
  delete m_UIExecutive;
  m_UIExecutive = 0;
  
  return AS_OK;
}
