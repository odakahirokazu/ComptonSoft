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
 * @date 2015-10-16 | for version 5
 */
class EfficiencyMapSky : public BackProjection
{
  DEFINE_ANL_MODULE(EfficiencyMapSky, 2.0)
public:
  EfficiencyMapSky();
  ~EfficiencyMapSky();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  
private:
  const anlgeant4::InitialInformation* m_InitialInfo;
  double m_Scale;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_EfficiencyMapSky_H */
