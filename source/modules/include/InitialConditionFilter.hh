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

#ifndef COMPTONSOFT_InitialConditionFilter_H
#define COMPTONSOFT_InitialConditionFilter_H 1

#include "BasicModule.hh"

namespace anlgeant4 {
class InitialInformation;
}

namespace comptonsoft {

class InitialConditionFilter : public anl::BasicModule
{
  DEFINE_ANL_MODULE(InitialConditionFilter, 1.1);
public:
  InitialConditionFilter();
  ~InitialConditionFilter() = default;

  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_analyze() override;

private:
  double energy0_;
  double energy1_;
  const anlgeant4::InitialInformation* initialInfo_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_InitialConditionFilter_H */
