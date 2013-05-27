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

#ifndef ANLGEANT4_VisualizeG4Geom_H
#define ANLGEANT4_VisualizeG4Geom_H 1

#include "BasicModule.hh"
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
 */
class VisualizeG4Geom  : public anl::BasicModule
{
  DEFINE_ANL_MODULE(VisualizeG4Geom, 1.2);
public: 
  VisualizeG4Geom();
  ~VisualizeG4Geom();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_endrun();
  anl::ANLStatus mod_exit(); 
  
private:
  G4VisManager* m_VisManager;
  G4UImanager* m_UIManager;
  G4UIExecutive* m_UIExecutive;

  std::string m_Mode;
  G4ThreeVector m_TargetPoint;
  G4ThreeVector m_ViewPoint;
  G4ThreeVector m_UpVector;
  double m_Zoom;
  bool m_AuxiliaryEdge;
};

}

#endif /* ANLGEANT4_VisualizeG4Geom_H */
