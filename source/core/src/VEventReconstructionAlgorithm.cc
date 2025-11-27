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

#include "VEventReconstructionAlgorithm.hh"
#include <boost/property_tree/json_parser.hpp>
#include "DetectorHit.hh"

namespace comptonsoft {

VEventReconstructionAlgorithm::VEventReconstructionAlgorithm()
  : minHits_(2), maxHits_(2)
{
}

VEventReconstructionAlgorithm::~VEventReconstructionAlgorithm() = default;

bool VEventReconstructionAlgorithm::readParameterFile()
{
  boost::property_tree::ptree pt;
  boost::property_tree::json_parser::read_json(ParameterFile().c_str(), pt);
  return loadParameters(pt);
}

double total_energy_deposits(const std::vector<DetectorHit_sptr>& hits)
{
  double sum = 0.0;
  for (const auto& hit: hits) { sum += hit->Energy(); }
  return sum;
}

} /* namespace comptonsoft */
