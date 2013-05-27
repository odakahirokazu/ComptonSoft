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

#ifndef COMPTONSOFT_EfficiencyMapSky_H
#define COMPTONSOFT_EfficiencyMapSky_H 1

#include "BackProjection.hh"

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {

/**
 * This module makes an efficiency image for Compton imaging reconstruction.
 * @author Hirokazu Odaka
 * @date 2012-03-13
 */
class EfficiencyMapSky : public BackProjection
{
  DEFINE_ANL_MODULE(EfficiencyMapSky, 1.0)
public:
  EfficiencyMapSky();
  ~EfficiencyMapSky() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  
private:
  const anlgeant4::InitialInformation* m_InitialInfo;
  double m_Scale;
};

}

#endif /* COMPTONSOFT_EfficiencyMapSky_H */
