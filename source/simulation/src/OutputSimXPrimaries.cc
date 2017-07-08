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

#include "OutputSimXPrimaries.hh"
#include "AstroUnits.hh"
#include "SimXIF.hh"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

OutputSimXPrimaries::OutputSimXPrimaries()
  : m_FileName("incident_photons.fits"), m_Area(300.0*unit::cm2)
{
}

ANLStatus OutputSimXPrimaries::mod_define()
{
  register_parameter(&m_FileName, "filename");
  register_parameter(&m_Area, "area", unit::cm2, "cm2");
  return AS_OK;
}

ANLStatus OutputSimXPrimaries::mod_initialize()
{
  SimXIF* simx = 0;
  get_module_NC("SimXIF", &simx);

  simx->generatePrimaries(m_Area);
  std::cout << "Number of primaries: " << simx->NumberOfPrimaries() << std::endl;
  simx->outputPrimaries(m_FileName);
  
  return AS_OK;
}

} /* namespace comptonsoft */
