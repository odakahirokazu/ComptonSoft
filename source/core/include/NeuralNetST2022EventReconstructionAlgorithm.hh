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

#ifndef COMPTONSOFT_NeuralNetST2022EventReconstructionAlgorithm_H
#define COMPTONSOFT_NeuralNetST2022EventReconstructionAlgorithm_H 1

#include "VEventReconstructionAlgorithm.hh"
#include <string>
#include <vector>
#include <memory>
#include "OnnxInference.hh"

namespace comptonsoft {

/**
 * class for Compton event reconstrcution
 * based on multi-task neural network
 * Reference: S. Takashima et al., NIMA, 1038, 166897, 2022
 *
 * @author Satoshi Takashima, Hirokazu Odaka
 * @date 2025-11-29
 */
class NeuralNetST2022EventReconstructionAlgorithm : public VEventReconstructionAlgorithm
{
public:
  NeuralNetST2022EventReconstructionAlgorithm();

  void initializeEvent() override;
  bool reconstruct(const std::vector<DetectorHit_sptr>& hits,
                   const BasicComptonEvent& baseEvent,
                   std::vector<BasicComptonEvent_sptr>& eventsReconstructed) override;

  std::tuple<bool, std::vector<DetectorHit_sptr>, double>
  infer_hit_order(const std::vector<DetectorHit_sptr>& hits);

protected:
  bool loadParameters(boost::property_tree::ptree& pt) override;

private:
  const int first_n_hits_;
  bool distinguish_escape_;
  std::vector<std::unique_ptr<OnnxInference>> models_;
  std::vector<std::vector<std::vector<int>>> permutations_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NeuralNetST2022EventReconstructionAlgorithm_H */
