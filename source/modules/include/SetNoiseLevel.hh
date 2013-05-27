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

#ifndef COMPTONSOFT_SetNoiseLevel_H
#define COMPTONSOFT_SetNoiseLevel_H 1

#include "VCSModule.hh"

#include <map>
#include "boost/tuple/tuple.hpp"


namespace comptonsoft {

class SetNoiseLevel : public VCSModule
{
  DEFINE_ANL_MODULE(SetNoiseLevel, 2.4);
public:
  SetNoiseLevel();
  ~SetNoiseLevel() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_init();

private:
  bool set_by_map();
  bool set_by_file();

  bool m_ByFile;
  std::string m_FileName;
  
  std::map<std::string, boost::tuple<int, double, double, double, double, double, double> > m_NoiseLevelMap;
  int m_DetectorType;
  double m_Noise00;
  double m_Noise01;
  double m_Noise02;
  double m_Noise10;
  double m_Noise11;
  double m_Noise12;
};

}

#endif /* COMPTONSOFT_SetNoiseLevel_H */
