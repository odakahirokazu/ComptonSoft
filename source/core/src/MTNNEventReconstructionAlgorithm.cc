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

#include "MTNNEventReconstructionAlgorithm.hh"
#include "AstroUnits.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

MTNNEventReconstructionAlgorithm::MTNNEventReconstructionAlgorithm()
  : distinguish_escape_(true),
    min_hits_(3),
    max_hits_(3)
{
  setParameterFile("parfile_MTNN.json");
}


MTNNEventReconstructionAlgorithm::~MTNNEventReconstructionAlgorithm()
{
  for (int i=0; i<num_models_; i++){
    delete analyzerNN_[i];
  }
}

bool MTNNEventReconstructionAlgorithm::loadParameters(boost::property_tree::ptree& pt)
{
  min_hits_ = pt.get<int>("min_hits");
  max_hits_ = MaxHits();
  distinguish_escape_ = pt.get<bool>("distinguish_escape");

  std::cout << distinguish_escape_ << std::endl;
  std::vector<double> source_dir;
  for (const auto& r: pt.get_child("source_dir")) {
    source_dir.push_back(r.second.get_value<double>());
  }
  set_source_direction(source_dir[0], source_dir[1], source_dir[2]);

  //Setup NN models
  std::string model_dir = pt.get<std::string>("model_dir");

  std::vector<std::string> model_filenames;
  for (const auto& r: pt.get_child("model_filenames")) {
    model_filenames.push_back(r.second.get_value<std::string>());
    std::cout << r.second.get_value<std::string>() << std::endl;
  }

  num_models_ = max_hits_ - min_hits_ + 1;
  if (num_models_ != (int)model_filenames.size()) {
    std::cout << "The number of models does not match the designated num_hits range" << std::endl;
    return false;
  }

  model_paths_.resize(num_models_);
  for (int ihit=min_hits_;ihit<=max_hits_;ihit++) {
    model_paths_[ihit - min_hits_] = model_dir + std::string("/") + model_filenames[ihit - min_hits_];
  }

  setupModel(model_paths_);

  std::cout << std::endl;
  std::cout << "--- MTNN (Multi-task neural network model) ---" << std::endl;
  std::cout << "source position: [" << source_dir[0] << ", "<< source_dir[1] << ", " << source_dir[2] << "]" << std::endl;
  std::cout << "distinguish_escape: " << distinguish_escape_ << std::endl;

  for (int ihit=min_hits_;ihit<=max_hits_;ihit++) {
    std::cout << "Model_path (" << ihit << "): ";
    std::cout << model_paths_[ihit-min_hits_] << std::endl;
  }

  return true;
}

void MTNNEventReconstructionAlgorithm::set_source_direction(float x, float y, float z)
{
  source_direction_.setX(x);
  source_direction_.setY(y);
  source_direction_.setZ(z);
  source_direction_ = source_direction_.unit();
}

void MTNNEventReconstructionAlgorithm::initializeEvent()
{
}

void MTNNEventReconstructionAlgorithm::setupModel(std::vector<std::string> model_paths)
{
  analyzerNN_.resize(num_models_, nullptr);

  for (int i=0;i<num_models_;i++) {
    int ihit = min_hits_ + i;
    const char* model_path_char = model_paths[i].c_str();
    analyzerNN_[i] = new NNAnalyzer(model_path_char, ihit);
    analyzerNN_[i]->SetupModel(distinguish_escape_);
  }
}

void MTNNEventReconstructionAlgorithm::setTotalEnergyDepositsAndNumHits(const std::vector<DetectorHit_sptr>& hits)
{
  total_energy_deposits_ = total_energy_deposits(hits);
  num_hits_ = hits.size();
}

bool MTNNEventReconstructionAlgorithm::
reconstruct(const std::vector<DetectorHit_sptr>& hits,
            const BasicComptonEvent& baseEvent,
            std::vector<BasicComptonEvent_sptr>& eventsReconstructed)
{
  auto eventReconstructed = std::make_shared<BasicComptonEvent>(baseEvent);
  setTotalEnergyDepositsAndNumHits(hits);

  if (num_hits_ > max_hits_ || num_hits_< min_hits_) {
    return false;
  }

  bool result = analyzerNN_[num_hits_ - min_hits_]->Run(hits, eventReconstructed);

  if (result) {
    eventsReconstructed.push_back(eventReconstructed);
  }

  return result;
}

} /* namespace comptonsoft */
