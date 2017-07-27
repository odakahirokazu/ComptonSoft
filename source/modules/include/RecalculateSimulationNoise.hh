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

#ifndef COMPTONSOFT_RecalculateSimulationNoise_H
#define COMPTONSOFT_RecalculateSimulationNoise_H 1

#include "VCSModule.hh"

namespace comptonsoft {

class CSHitCollection;

/**
 * recalculate simulation noise in order to get new PIs.
 * @author Hirokazu Odaka
 * @date 2011-02-16
 * @date 2014-11-26
 */
class RecalculateSimulationNoise : public VCSModule
{
  DEFINE_ANL_MODULE(RecalculateSimulationNoise, 2.1);
public:
  RecalculateSimulationNoise();
  ~RecalculateSimulationNoise() = default;

  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  
private:
  CSHitCollection* m_HitCollection;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RecalculateSimulationNoise_H */
