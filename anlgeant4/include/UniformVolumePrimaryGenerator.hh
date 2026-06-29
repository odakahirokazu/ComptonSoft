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

#include "PrimarySetting.hh"
#ifndef ANLGEANT4_UniformVolumePrimaryGenerator_H
#define ANLGEANT4_UniformVolumePrimaryGenerator_H 1

#include <vector>
#include "PointSourcePrimaryGenerator.hh"
#include "PositionSamplerInVolume.hh"

namespace anlgeant4
{

/**
 *
 * @date 2011-06-22 | Hirokazu Odaka
 * @date 2017-06-27 | Hirokazu Odaka | 4.1, makePrimarySetting()
 */
class UniformVolumePrimaryGenerator : public PointSourcePrimaryGenerator
{
  DEFINE_ANL_MODULE(UniformVolumePrimaryGenerator, 7.0);
public:
  UniformVolumePrimaryGenerator();
  ~UniformVolumePrimaryGenerator() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;

  PrimarySetting make_primary_setting() const override;
  G4ThreeVector sample_position() const override;

private:
  PositionSamplerInVolume position_sampler_;
  bool target_mode_;
  G4ThreeVector target_position_;
  std::vector<std::string> volume_hierarchy_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_UniformVolumePrimaryGenerator_H */
