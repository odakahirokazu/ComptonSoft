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

#ifndef COMPTONSOFT_WeightByInitialDirection_H
#define COMPTONSOFT_WeightByInitialDirection_H 1

#include "BasicModule.hh"
#include <memory>

class TGraph;
namespace anlgeant4 { class InitialInformation; }

namespace comptonsoft {

/**
 *
 * @author Hirokazu Odaka
 * @date 2017-04-27
 */
class WeightByInitialDirection : public anl::BasicModule
{
  DEFINE_ANL_MODULE(WeightByInitialDirection, 1.0);
public:
  WeightByInitialDirection();
  ~WeightByInitialDirection() = default;

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();
  
private:
  std::vector<double> m_DirZVector;
  std::vector<double> m_WeightVector;

  anlgeant4::InitialInformation* m_InitialInfo = nullptr;
  std::unique_ptr<TGraph> m_WeightFunction;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_WeightByInitialDirection_H */
