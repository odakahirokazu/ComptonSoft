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

#include "WeightByInitialDirection.hh"

#include "TGraph.h"
#include "InitialInformation.hh"

using namespace anlnext;

namespace comptonsoft
{

WeightByInitialDirection::WeightByInitialDirection()
{
}

ANLStatus WeightByInitialDirection::mod_define()
{
  define_parameter("dirz_vector", &mod_class::dirz_vector_);
  define_parameter("weight_vector", &mod_class::weight_vector_);
  return AS_OK;
}

ANLStatus WeightByInitialDirection::mod_initialize()
{
  get_module_IFNC("InitialInformation", &initial_info_);
  weight_function_.reset(new TGraph(dirz_vector_.size(), &dirz_vector_[0], &weight_vector_[0]));
  return AS_OK;
}

ANLStatus WeightByInitialDirection::mod_analyze()
{
  const G4ThreeVector dir0 = initial_info_->initial_direction();
  const double weight0 = initial_info_->weight();
  const double weight1 = weight0 * (weight_function_->Eval(dir0.z()));
  initial_info_->set_weight(weight1);
  return AS_OK;
}

} /* namespace comptonsoft */
