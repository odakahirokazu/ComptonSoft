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

#ifndef ANLGEANT4_VisualizeGeometry_H
#define ANLGEANT4_VisualizeGeometry_H 1

#include <memory>
#include <anlnext/BasicModule.hh>
#include "G4ThreeVector.hh"

class G4VisManager;
class G4UImanager;
class G4UIExecutive;


namespace anlgeant4 {


/**
 * Geant4 visualization for Compton Soft
 * @author Hirokazu Odaka
 * @date 2011-09-28
 * @date 2012-02-15
 * @date 2012-08-06
 * @date 2012-10-04
 * @date 2016-09-06
 * @date 2017-06-21 | explicit delete of the visualization manager.
 */
class VisualizeGeometry  : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(VisualizeGeometry, 1.5);
public:
  VisualizeGeometry();
  ~VisualizeGeometry();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

private:
  void applyDefaultCommands();

private:
  std::unique_ptr<G4VisManager> vis_manager_;
  std::unique_ptr<G4UIExecutive> UI_executive_;
  G4UImanager* UI_manager_;

  std::string mode_;
  G4ThreeVector target_point_;
  G4ThreeVector view_point_;
  G4ThreeVector up_vector_;
  double zoom_;
  bool auxiliary_edge_;
  std::string macro_file_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VisualizeGeometry_H */
