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

#ifndef COMPTONSOFT_MTNNEventReconstructionAlgorithm_H
#define COMPTONSOFT_MTNNEventReconstructionAlgorithm_H 1

#include "VEventReconstructionAlgorithm.hh"
#include <string>
#include <vector>
#include <CLHEP/Vector/ThreeVector.h>
#include "DetectorHit_sptr.hh"
#include "NNAnalyzer.hh"
#include <boost/property_tree/json_parser.hpp>

namespace comptonsoft {

class MTNNEventReconstructionAlgorithm : public VEventReconstructionAlgorithm
{
public:
  MTNNEventReconstructionAlgorithm();
  virtual ~MTNNEventReconstructionAlgorithm();
  MTNNEventReconstructionAlgorithm(const MTNNEventReconstructionAlgorithm&) = default;
  MTNNEventReconstructionAlgorithm(MTNNEventReconstructionAlgorithm&&) = default;
  MTNNEventReconstructionAlgorithm& operator=(const MTNNEventReconstructionAlgorithm&) = default;
  MTNNEventReconstructionAlgorithm& operator=(MTNNEventReconstructionAlgorithm&&) = default;

  bool loadParameters(boost::property_tree::ptree& pt);
  void set_source_direction(float x, float y, float z);
  void initializeEvent();
  void setupModel(std::vector<std::string> model_paths);
  void setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits);
  bool reconstruct(const std::vector<DetectorHit_sptr>& hits,
                   const BasicComptonEvent& baseEvent,
                   std::vector<BasicComptonEvent_sptr>& eventsReconstructed);

private:
  bool distinguish_escape_;
  int min_hits_, max_hits_;
  int num_models_;
  std::vector<std::string> model_paths_;

  vector3_t source_direction_;

  double total_energy_deposits_;
  int num_hits_;

  std::vector<NNAnalyzer*> analyzerNN_;
};


} /* namespace comptonsoft */

#endif /* COMPTONSOFT_MTNNEventReconstructionAlgorithm_H */
