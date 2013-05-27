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

#ifndef ANLGEANT4_WriteGDML_H
#define ANLGEANT4_WriteGDML_H 1

#include "BasicModule.hh"

namespace anlgeant4 {

/**
 * ANL module for writing a GDML file
 * @author Hirokazu Odaka
 * @date 2011-07-30
 */
class WriteGDML : public anl::BasicModule
{
  DEFINE_ANL_MODULE(WriteGDML, 1.0);
public:
  WriteGDML();
  
  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();

private:
  std::string m_GeometryFileName;
};

}

#endif /* ANLGEANT4_WriteGDML_H */
