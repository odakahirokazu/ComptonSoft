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

#ifndef ANLGEANT4_ReadGDML_H
#define ANLGEANT4_ReadGDML_H 1

#include "VANLGeometry.hh"

class G4VUserDetectorConstruction;

namespace anlgeant4 {

/**
 * GDML Geometry module
 * @author Hirokazu Odaka
 * @date 2011-07-30
 */
class ReadGDML : public VANLGeometry
{
  DEFINE_ANL_MODULE(ReadGDML, 1.0);

public:
  ReadGDML();

  anl::ANLStatus mod_startup() override;

  G4VUserDetectorConstruction* create() override;

private:
  std::string m_GeometryFileName;
  bool m_Validate;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_ReadGDML_H */
