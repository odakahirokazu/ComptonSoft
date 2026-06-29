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

#include "VisualizeGeometry.hh"

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

VisualizeGeometry::VisualizeGeometry()
  : UI_manager_(nullptr),
    mode_("OGL"),
    target_point_(0.0, 0.0, 0.0),
    view_point_(0.0, 1.0, 0.0),
    up_vector_(0.0, 0.0, 1.0),
    zoom_(1.0),
    auxiliary_edge_(true),
    macro_file_("")
{
}

VisualizeGeometry::~VisualizeGeometry() = default;

ANLStatus VisualizeGeometry::mod_define()
{
  define_parameter("mode", &mod_class::mode_);
  define_parameter("target_point", &mod_class::target_point_, unit::cm, "cm");
  define_parameter("view_point", &mod_class::view_point_);
  define_parameter("up_direction", &mod_class::up_vector_);
  define_parameter("zoom", &mod_class::zoom_);
  define_parameter("auxiliary_edge", &mod_class::auxiliary_edge_);
  define_parameter("macro_file", &mod_class::macro_file_);

  return AS_OK;
}

ANLStatus VisualizeGeometry::mod_initialize()
{
  vis_manager_.reset(new G4VisExecutive);
  UI_manager_ = G4UImanager::GetUIpointer();
  vis_manager_->Initialize();

  static char* argv[1];
  static char name[16] = "ANLGeant4";
  argv[0] = name;
  UI_executive_.reset(new G4UIExecutive(1, argv));

  if (macro_file_=="") {
    applyDefaultCommands();
  }
  else {
    std::ostringstream cmd;
    cmd << "/control/execute " << macro_file_;
    UI_manager_->ApplyCommand(cmd.str());
  }

  return AS_OK;
}

ANLStatus VisualizeGeometry::mod_end_run()
{
  if (mode_.find("OGL") != std::string::npos) {
    UI_executive_->SessionStart();
  }

  return AS_OK;
}

ANLStatus VisualizeGeometry::mod_finalize()
{
  UI_executive_.reset();
  vis_manager_.reset();

  return AS_OK;
}

void VisualizeGeometry::applyDefaultCommands()
{
  UI_manager_->ApplyCommand("/control/verbose 2");

  std::ostringstream cmd;
  cmd.str("");
  cmd << "/vis/open " << mode_ << " 600x600-0+0";
  UI_manager_->ApplyCommand(cmd.str());
  cmd.str("");

  UI_manager_->ApplyCommand("/vis/viewer/set/autoRefresh false");
  UI_manager_->ApplyCommand("/vis/verbose errors");
  UI_manager_->ApplyCommand("/vis/drawVolume worlds");

  cmd.str("");
  cmd << "/vis/viewer/set/targetPoint "
      << target_point_.x()/unit::cm << " "
      << target_point_.y()/unit::cm << " "
      << target_point_.z()/unit::cm << " cm";
  UI_manager_->ApplyCommand(cmd.str());
  cmd.str("");

  cmd.str("");
  cmd << "/vis/viewer/set/viewpointVector "
      << view_point_.x() << " "
      << view_point_.y() << " "
      << view_point_.z();
  UI_manager_->ApplyCommand(cmd.str());
  cmd.str("");

  cmd.str("");
  cmd << "/vis/viewer/set/upVector "
      << up_vector_.x() << " "
      << up_vector_.y() << " "
      << up_vector_.z();
  UI_manager_->ApplyCommand(cmd.str());
  cmd.str("");

  cmd << "/vis/viewer/zoom "<< zoom_;
  UI_manager_->ApplyCommand(cmd.str());
  cmd.str("");

  cmd << "/vis/viewer/set/auxiliaryEdge "<< auxiliary_edge_;
  UI_manager_->ApplyCommand(cmd.str());
  cmd.str("");

  UI_manager_->ApplyCommand("/vis/viewer/set/style wireframe");
  UI_manager_->ApplyCommand("/vis/scene/add/trajectories smooth");
  UI_manager_->ApplyCommand("/vis/scene/add/hits");

  UI_manager_->ApplyCommand("/vis/modeling/trajectories/create/drawByParticleID");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set e- red");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set e+ blue");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set proton cyan");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set gamma green");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set neutron yellow");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi+ magenta");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi- magenta");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/set pi0 magenta");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/default/setDrawStepPts true");
  UI_manager_->ApplyCommand("/vis/modeling/trajectories/drawByParticleID-0/default/setStepPtsSize 2");

  UI_manager_->ApplyCommand("/vis/scene/endOfEventAction accumulate");
  UI_manager_->ApplyCommand("/vis/viewer/set/autoRefresh true");
  UI_manager_->ApplyCommand("/vis/verbose warnings");

  UI_manager_->ApplyCommand("/vis/viewer/flush");

  if (UI_executive_->IsGUI()) {
    UI_manager_->ApplyCommand("/gui/addMenu viewer Viewer");
    UI_manager_->ApplyCommand("/gui/addButton viewer \"Set style surface\" \"/vis/viewer/set/style s\"");
    UI_manager_->ApplyCommand("/gui/addButton viewer \"Set style wireframe\" \"/vis/viewer/set/style w\"");
  }
}

} /* namespace anlgeant4 */
