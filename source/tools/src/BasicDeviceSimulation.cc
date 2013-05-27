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

#include "BasicDeviceSimulation.hh"

#include "CLHEP/Random/RandGauss.h"
#include "cs_globals.hh"
#include "DetectorHit.hh"
#include "HXISGDUtility.hh"
#include "HXISGDFlagDef.hh"


namespace comptonsoft {

BasicDeviceSimulation::BasicDeviceSimulation()
{
}


BasicDeviceSimulation::~BasicDeviceSimulation()
{
}


void BasicDeviceSimulation::initializeTable()
{
  VDeviceSimulation::initializeTable();
}


double BasicDeviceSimulation::calculatePHA(const StripPair& , double edep, double , double , double )
{
  return edep;
}


double BasicDeviceSimulation::calculatePI(double pha, const StripPair& sp)
{
  return VDeviceSimulation::calculatePI(pha, sp);
}


void BasicDeviceSimulation::printSimParam()
{
  std::cout << "Quenching factor : " << QuenchingFactor() << std::endl;
}

}
