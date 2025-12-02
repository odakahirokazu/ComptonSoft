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

#include "NeuralNetST2022EventReconstructionAlgorithm.hh"

#include "DetectorHit.hh"
#include "AstroUnits.hh"

namespace unit = anlgeant4::unit;
namespace constant = anlgeant4::constant;

namespace {

std::vector<std::vector<int>>
k_permutations(int n, int k) {
  std::vector<std::vector<int>> result;
  if (k > n || k < 0) { return result; }
  
  std::vector<bool> used(n, false);
  std::vector<int> a(k);

  std::function<void(int)> dfs = [&](int depth) {
    if (depth == k) {
      result.push_back(a);
      return;
    }
    for (int i=0; i<n; ++i) {
      if (used[i]) { continue; }
      used[i] = true;
      a[depth] = i;
      dfs(depth + 1);
      used[i] = false;
    }
  };

  dfs(0);
  return result;
}

} /* anonymous namespace */


namespace comptonsoft {

NeuralNetST2022EventReconstructionAlgorithm::NeuralNetST2022EventReconstructionAlgorithm()
  : first_n_hits_(3), distinguish_escape_(true)
{
  setParameterFile("parfile_MTNN.json");
}


bool NeuralNetST2022EventReconstructionAlgorithm::loadParameters(boost::property_tree::ptree& pt)
{
  distinguish_escape_ = pt.get<bool>("distinguish_escape");
  std::cout << distinguish_escape_ << std::endl;

  const std::string model_dir = pt.get<std::string>("model_dir");
  std::vector<std::string> model_paths;
  for (const auto& r: pt.get_child("model_filenames")) {
    const std::string filename = r.second.get_value<std::string>();
    const std::string path = model_dir + std::string("/") + filename;
    model_paths.push_back(path);
  }

  const size_t num_models = MaxHits() - (MinHits()-1);
  if (num_models != model_paths.size()) {
    std::cout << "The number of models does not match the designated num_hits range" << std::endl;
    return false;
  }

  models_.reserve(num_models);
  for (const std::string& path: model_paths) {
    models_.emplace_back(std::make_unique<OnnxInference>(path));
  }

  permutations_.reserve(num_models);
  for (int n=MinHits(); n<=MaxHits(); ++n) {
    permutations_.push_back(k_permutations(n, first_n_hits_));
  }

  std::cout << std::endl;
  std::cout << "--- Multi-task neural network model ST2022 ---" << std::endl;
  std::cout << "distinguish_escape: " << distinguish_escape_ << std::endl;

  for (size_t i=0; i<num_models; ++i) {
    const int nhit = MinHits() + i;
    std::cout << "Model_path (" << nhit << "): ";
    std::cout << model_paths[i] << std::endl;
  }

#if 0
  for (size_t i=0; i<num_models; ++i) {
    const int nhit = MinHits() + i;
    std::cout << "Permutations (" << nhit << "): " << std::endl;
    for (const auto& v: permutations_[i]) {
      for (const int& k: v) {
        std::cout << k << " ";
      }
      std::cout << std::endl;
    }
  }
#endif

  return true;
}

void NeuralNetST2022EventReconstructionAlgorithm::initializeEvent()
{
}

bool NeuralNetST2022EventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  const int num_hits = hits.size();
  if (num_hits < MinHits() || num_hits > MaxHits()) {
    return false;
  }

  // infer the escape flag and hit order via multi-task neural network
  const auto [escape_flag, ordered_hits, order_index, order_probability] = infer_hit_order(hits);

  auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);
  eventReconstructed->setHit1(0, ordered_hits[0]);
  eventReconstructed->setHit2(1, ordered_hits[1]);
  eventReconstructed->setNumberOfHits(num_hits);
  eventReconstructed->setEscapeFlag(escape_flag);
  eventReconstructed->setLikelihood(order_probability);
  eventReconstructed->setReconstructedOrder(order_index);
  eventReconstructed->setReconstructionFraction(1.0);

  const double sum_energies = total_energy_deposits(hits);
  eventReconstructed->setTotalEnergyDeposit(sum_energies);

  if (distinguish_escape_ && !escape_flag) {
    const double energy_recoil_electron = ordered_hits[0]->Energy();
    const double energy_scattered_photon = sum_energies - energy_recoil_electron;
    eventReconstructed->setHit2Energy(energy_scattered_photon);
  }
  else {
    const vector3_t first_scat_dir = (ordered_hits[1]->Position() - ordered_hits[0]->Position()).unit();
    const vector3_t second_scat_dir = (ordered_hits[2]->Position() - ordered_hits[1]->Position()).unit();

    constexpr double epsilon = 1.0e-9;

    // scattering angle theta of the second scattering
    const double costheta2 = first_scat_dir.dot(second_scat_dir);
    // energy of the recoil electron of the second scattering
    const double e2 = ordered_hits[1]->Energy();

    const double energy_second_scattered_photon
      = - 0.5*e2
      + std::sqrt(0.25*e2*e2 + constant::electron_mass_c2*e2 / (1.0-costheta2+epsilon));
    const double energy_first_scattered_photon = e2 + energy_second_scattered_photon;
    eventReconstructed->setHit2Energy(energy_first_scattered_photon);
  }
  
  eventsReconstructed.push_back(eventReconstructed);
  return true;
}

std::tuple<bool, std::vector<DetectorHit_sptr>, int, double>
NeuralNetST2022EventReconstructionAlgorithm::infer_hit_order(const std::vector<DetectorHit_sptr>& hits)
{
  std::vector<DetectorHit_sptr> energy_sorted_hits = hits;
  std::sort(energy_sorted_hits.begin(), energy_sorted_hits.end(),
            [](const DetectorHit_sptr& a, const DetectorHit_sptr& b) {
              return a->Energy() > b->Energy();
            });

  const size_t num_hits = hits.size();
  const size_t model_index = num_hits - MinHits();
  OnnxInference* model = models_[model_index].get();
  const std::vector<std::vector<int>>& permutations = permutations_[model_index];

  const size_t num_params_per_hit = 4;
  const size_t input_dim = num_params_per_hit * num_hits;
  std::vector<float> input (input_dim, 0.0);
  for (size_t i=0; i<num_hits; ++i) {
    input[i*num_params_per_hit + 0] = energy_sorted_hits[i]->Energy() / unit::keV;
    input[i*num_params_per_hit + 1] = energy_sorted_hits[i]->PositionX() / unit::cm;
    input[i*num_params_per_hit + 2] = energy_sorted_hits[i]->PositionY() / unit::cm;
    input[i*num_params_per_hit + 3] = energy_sorted_hits[i]->PositionZ() / unit::cm;
  }

  const std::vector<std::vector<float>> output = model->infer(input);
  const std::vector<float>& output_escape = output[0];
  const std::vector<float>& output_order  = output[1];
  
  const bool escape_flag = (output_escape[0] < output_escape[1]) ? true: false;

  const auto order_iter = std::max_element(output_order.begin(), output_order.end());
  const int order_index = std::distance(output_order.begin(), order_iter);
  const double order_probability = *order_iter;
  const std::vector<int>& order = permutations[order_index];

  std::vector<DetectorHit_sptr> ordered_hits;
  for (int i=0; i<first_n_hits_; ++i) {
    ordered_hits.push_back(energy_sorted_hits[order[i]]);
  }
 
  return std::make_tuple(escape_flag, ordered_hits, order_index, order_probability);
}

} /* namespace comptonsoft */
