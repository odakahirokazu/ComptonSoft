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

#ifndef ANLGEANT4_NucleusPrimaryInVolume_H
#define ANLGEANT4_NucleusPrimaryInVolume_H 1

#include <string>
#include <vector>
#include "NucleusPrimaryGen.hh"
#include "PositionSamplerInVolume.hh"

namespace anlgeant4 {

/**
 *
 * @date 2008-09-19 Hirokazu Odaka
 * @date 2011-04-12 Hirokazu Odaka
 * @date 2011-06-22 Hirokazu Odaka
 * @date 2017-06-27 Hirokazu Odaka | tweak
 */
class NucleusPrimaryGenInVolume : public NucleusPrimaryGen
{
  DEFINE_ANL_MODULE(NucleusPrimaryGenInVolume, 4.1);
public: 
  NucleusPrimaryGenInVolume();
  ~NucleusPrimaryGenInVolume();

  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_com() override;
  anl::ANLStatus mod_bgnrun() override;

  void makePrimarySetting() override;

private:
  anlgeant4::PositionSamplerInVolume m_PositionSampler;
  std::vector<std::string> m_VolumeHierarchy;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_NucleusPrimaryGenInVolume_H */
