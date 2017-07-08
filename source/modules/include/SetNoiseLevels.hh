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

#ifndef COMPTONSOFT_SetNoiseLevels_H
#define COMPTONSOFT_SetNoiseLevels_H 1

#include "VCSModule.hh"
#include <map>
#include <tuple>

namespace comptonsoft {

class SetNoiseLevels : public VCSModule
{
  DEFINE_ANL_MODULE(SetNoiseLevels, 2.5);
public:
  SetNoiseLevels();
  ~SetNoiseLevels();

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;

private:
  bool set_by_map();
  bool set_by_file();

  bool m_ByFile;
  std::string m_FileName;
  
  std::map<std::string, std::tuple<int,
                                   double, double, double,
                                   double, double, double,
                                   double, double, double>> m_NoiseLevelMap;
  int m_DetectorType;
  double m_Noise0;
  double m_Noise1;
  double m_Noise2;
  double m_CathodeNoise0;
  double m_CathodeNoise1;
  double m_CathodeNoise2;
  double m_AnodeNoise0;
  double m_AnodeNoise1;
  double m_AnodeNoise2;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetNoiseLevels_H */
