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

#ifndef ANLGEANT4_PrimaryGenUniformSourceInVolume_H
#define ANLGEANT4_PrimaryGenUniformSourceInVolume_H 1

#include <vector>
#include "PointSourcePrimaryGen.hh"
#include "PositionSamplerInVolume.hh"

namespace anlgeant4
{

/**
 *
 * @date 2011-06-22 | Hirokazu Odaka
 * @date 2017-06-27 | Hirokazu Odaka | 4.1, makePrimarySetting()
 */
class PrimaryGenUniformSourceInVolume : public PointSourcePrimaryGen
{
  DEFINE_ANL_MODULE(PrimaryGenUniformSourceInVolume, 4.1);
public: 
  PrimaryGenUniformSourceInVolume();
  ~PrimaryGenUniformSourceInVolume() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;

  void makePrimarySetting() override;

private:
  PositionSamplerInVolume m_PositionSampler;
  bool m_TargetMode;
  G4ThreeVector m_TargetPosition;
  std::vector<std::string> m_VolumeHierarchy;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_PrimaryGenUniformSourceInVolume_H */
