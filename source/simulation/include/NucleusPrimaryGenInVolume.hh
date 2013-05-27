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

// 2008-09-19 Hirokazu Odaka
// 2011-04-12 Hirokazu Odaka
// 2011-06-22 Hirokazu Odaka

#ifndef COMPTONSOFT_NucleusPrimaryInVolume_H
#define COMPTONSOFT_NucleusPrimaryInVolume_H 1

#include <string>
#include <vector>
#include "NucleusPrimaryGen.hh"
#include "VolumeInsideSample.hh"

namespace comptonsoft {


class NucleusPrimaryGenInVolume : public NucleusPrimaryGen
{
  DEFINE_ANL_MODULE(NucleusPrimaryGenInVolume, 1.2);
public: 
  NucleusPrimaryGenInVolume();
  virtual ~NucleusPrimaryGenInVolume(){}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_bgnrun();
  anl::ANLStatus mod_ana();

private:
  VolumeInsideSample m_PositionSampler;
  std::vector<std::string> m_VolumeHierarchy;
};

}

#endif /* COMPTONSOFT_NucleusPrimaryGenInVolume_H */
