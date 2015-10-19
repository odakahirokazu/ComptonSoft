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

// ConstructDetectorForSimulation.hh
// 2008-08-27 Hirokazu Odaka

#ifndef COMPTONSOFT_ConstructDetectorForSimulation_H
#define COMPTONSOFT_ConstructDetectorForSimulation_H 1

#include "ConstructDetector.hh"

namespace comptonsoft {

class ConstructDetectorForSimulation : public ConstructDetector
{
  DEFINE_ANL_MODULE(ConstructDetectorForSimulation, 3.1);
public:
  ConstructDetectorForSimulation();
  ~ConstructDetectorForSimulation();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_bgnrun();

private:
  std::string simulationParamsFile_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ConstructDetectorForSimulation_H */
