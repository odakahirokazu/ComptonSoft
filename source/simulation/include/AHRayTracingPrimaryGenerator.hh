/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#ifndef COMPTONSOFT_AHRayTracingPrimaryGenerator_H
#define COMPTONSOFT_AHRayTracingPrimaryGenerator_H 1

#include <vector>
#include <mutex>
#include "BasicPrimaryGenerator.hh"

namespace comptonsoft {


/**
 * ANLGeant4 PrimaryGenerator module.
 * Primary particles are generated according to a list of ray-tracing output.
 *
 * @author Tamotsu Sato, Hirokazu Odaka
 * @date 2011-11-07
 * @date 2012-06-13
 * @date 2012-09-14
 * @date 2017-07-27 | makePrimarySetting()
 * @date 2019-07-01 | m_EnergyResample by Tsubasa Tamba
 */
class AHRayTracingPrimaryGenerator : public anlgeant4::BasicPrimaryGenerator
{
  DEFINE_ANL_MODULE(AHRayTracingPrimaryGenerator, 4.1);
public:
  AHRayTracingPrimaryGenerator();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

  anlgeant4::PrimarySetting make_primary_setting() const override;

private:
  std::string m_FileName;

  int m_EventNum;
  mutable int m_ID;
  mutable std::mutex mutex_;

  static const int NumColumns = 6;
  std::vector<double> m_Columns[6];
  G4ThreeVector m_offset;

  bool m_EnergyResample = false;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AHRayTracePrimaryGenerator_H */
