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

#ifndef COMPTONSOFT_RescaleSimulationNoiseOfSGDSiUntriggered_H
#define COMPTONSOFT_RescaleSimulationNoiseOfSGDSiUntriggered_H 1

#include "VCSModule.hh"

namespace comptonsoft {

class CSHitCollection;

/**
 * rescale simulation noise of untriggered hits in SGD Si-sensors.
 * @author Hirokazu Odaka
 * @date 2017-03-30
 */
class RescaleSimulationNoiseOfSGDSiUntriggered : public VCSModule
{
  DEFINE_ANL_MODULE(RescaleSimulationNoiseOfSGDSiUntriggered, 1.0);
public:
  RescaleSimulationNoiseOfSGDSiUntriggered();
  ~RescaleSimulationNoiseOfSGDSiUntriggered() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  
private:
  double m_Scale;
  CSHitCollection* m_HitCollection;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RescaleSimulationNoiseOfSGDSiUntriggered_H */
