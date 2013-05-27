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

#ifndef COMPTONSOFT_SetSimGainCorrection_H
#define COMPTONSOFT_SetSimGainCorrection_H 1

#include "VCSModule.hh"

#include <map>
#include <boost/tuple/tuple.hpp>

class TFile;


namespace comptonsoft {

class SetSimGainCorrection : public VCSModule
{
  DEFINE_ANL_MODULE(SetSimGainCorrection, 1.0);
public:
  SetSimGainCorrection();
  ~SetSimGainCorrection() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_com();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_exit();

private:
  bool set_by_map();
  bool set_by_file();

  bool m_ByFile;
  std::string m_FileName;
  TFile* m_FunctionFile;
  std::map<std::string,
           boost::tuple<int, std::string, std::string, double, double> > m_GainMap;
  int m_Type;
  std::string m_GainFunction0;
  std::string m_GainFunction1;
  double m_GainCoeff0;
  double m_GainCoeff1;
};

}

#endif /* COMPTONSOFT_SetSimGainCorrection_H */
