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

#include "WeightByInitialDirection.hh"

#include "TGraph.h"
#include "InitialInformation.hh"

using namespace anl;

namespace comptonsoft
{

WeightByInitialDirection::WeightByInitialDirection()
{
}

ANLStatus WeightByInitialDirection::mod_startup()
{
  register_parameter(&m_DirZVector, "dirz_vector");
  register_parameter(&m_WeightVector, "weight_vector");
  return AS_OK;
}

ANLStatus WeightByInitialDirection::mod_init()
{
  GetANLModuleIFNC("InitialInformation", &m_InitialInfo);
  m_WeightFunction.reset(new TGraph(m_DirZVector.size(), &m_DirZVector[0], &m_WeightVector[0]));
  return AS_OK;
}

ANLStatus WeightByInitialDirection::mod_ana()
{
  const G4ThreeVector dir0 = m_InitialInfo->InitialDirection();
  const double weight0 = m_InitialInfo->Weight();
  const double weight1 = weight0 * (m_WeightFunction->Eval(dir0.z()));
  m_InitialInfo->setWeight(weight1);
  return AS_OK;
}

} /* namespace comptonsoft */
