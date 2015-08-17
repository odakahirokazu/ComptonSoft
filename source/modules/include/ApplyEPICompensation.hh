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

#ifndef COMPTONSOFT_ApplyEPICompensation_H
#define COMPTONSOFT_ApplyEPICompensation_H 1

#include "VCSModule.hh"

#include <map>
#include <tuple>

class TFile;

namespace comptonsoft {

/**
 * @author Hirokazu Odaka
 * @date 2015-05-14 | rename from SetSimGainCorrection.
 */
class ApplyEPICompensation : public VCSModule
{
  DEFINE_ANL_MODULE(ApplyEPICompensation, 2.0);
public:
  ApplyEPICompensation();
  ~ApplyEPICompensation();

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
           std::tuple<int, double, double>> m_Map;
  int m_Type;
  double m_Factor0;
  double m_Factor1;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ApplyEPICompensation_H */
