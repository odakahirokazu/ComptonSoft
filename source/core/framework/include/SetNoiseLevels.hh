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

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;

private:
  bool set_by_map();
  bool set_by_file();

  bool setting_by_file_;
  std::string filename_;

  std::map<std::string, std::tuple<int,
                                   double, double, double,
                                   double, double, double,
                                   double, double, double>> noise_level_map_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetNoiseLevels_H */
