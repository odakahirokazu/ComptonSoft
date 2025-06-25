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

#include "AstroUnits.hh"
#include "NNAnalyzer.hh"

namespace unit = anlgeant4::unit;
namespace constant = anlgeant4::constant;

namespace comptonsoft {


NNAnalyzer::NNAnalyzer(const char* trained_model_path, int model_num_hits)
{
  model_num_hits_         = model_num_hits;
  trained_model_path_str_ = std::string(trained_model_path);

  std::tie(std::ignore, dict_label2array_)
    = label_dictionary_firstNhits(model_num_hits_, first_n_hits_);

  num_order_combinations_ = dict_label2array_.size();
}


NNAnalyzer::~NNAnalyzer()
{
  delete alloc_;
  delete session_;
  delete session_options_;
  delete memory_info_;
  delete env_;
}


void NNAnalyzer::SetupModel(bool distinguish_escape)
{
  const char* trained_model_path_name = trained_model_path_str_.c_str();
  distinguish_escape_                 = distinguish_escape;

  //Initialize session
  env_         = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, "test");
  memory_info_ = new Ort::MemoryInfo(Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU));

  session_options_ = new Ort::SessionOptions();
  try {
    session_ = new Ort::Session(*env_, trained_model_path_name, *session_options_);
  }
  catch(Ort::Exception& e) {
    std::cout << e.what() << std::endl;
    return;
  }
  alloc_ = new Ort::AllocatorWithDefaultOptions();

  size_t dim_model_input = session_->GetInputCount();
  for (size_t i=0; i < dim_model_input; ++i) {
    inputNames_.emplace_back(session_->GetInputNameAllocated(i, *alloc_));
  }

  for (std::size_t i=0; i<dim_model_input; i++) {
    input_names_.emplace_back(session_->GetInputNameAllocated(i, *alloc_).get());
    input_shape_ = session_->GetInputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
  }

  //escape flag
  output_names_.emplace_back(session_->GetOutputNameAllocated(0, *alloc_).get());
  output_shapes_escape_flag_ = session_->GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();

  output_escape_flag_values_.resize(2, 0.0);
  output_escape_flag_shape_.resize(2, 0);

  //hit order
  output_names_.emplace_back(session_->GetOutputNameAllocated(1, *alloc_).get());
  output_shapes_hit_order_ = session_->GetOutputTypeInfo(1).GetTensorTypeAndShapeInfo().GetShape();

  output_hit_order_values_.resize(num_order_combinations_, 0.0);
  output_hit_order_shape_.resize(2, 0);

  // pass data through model
  input_names_char_.resize(input_names_.size(), nullptr);
  std::transform(std::begin(input_names_), std::end(input_names_), std::begin(input_names_char_),
                 [&](const std::string& str) { return str.c_str(); });

  output_names_char_.resize(output_names_.size(), nullptr);
  std::transform(std::begin(output_names_), std::end(output_names_), std::begin(output_names_char_),
                 [&](const std::string& str) { return str.c_str(); });

  dim_input_ = calculate_product(input_shape_);
  input_values_.resize(dim_input_, 0.0);

  std::generate(input_values_.begin(), input_values_.end(), [&] { return rand() % 255; });
  input_tensors_.emplace_back(vec_to_tensor<float>(input_values_, input_shape_));

  outputTensor_hit_order_ = Ort::Value::CreateTensor<float>(*memory_info_,
                                                            output_hit_order_values_.data(), output_hit_order_values_.size(),
                                                            output_hit_order_shape_.data(),  output_hit_order_shape_.size());
  outputTensor_escape_flag_ = Ort::Value::CreateTensor<float>(*memory_info_,
                                                              output_escape_flag_values_.data(), output_escape_flag_values_.size(),
                                                              output_escape_flag_shape_.data(),  output_escape_flag_shape_.size());

  output_tensor_hit_order_values_.resize(num_order_combinations_, 0.0);
  output_tensor_escape_flag_values_.resize(2, 0.0);
  outputTensors_.emplace_back(vec_to_tensor<float>(output_tensor_escape_flag_values_,
                                                   output_shapes_escape_flag_));
  outputTensors_.emplace_back(vec_to_tensor<float>(output_tensor_hit_order_values_,
                                                   output_shapes_hit_order_));

  //Prepare output data
  E_vec_.resize(    model_num_hits_, 0.0 );
  posx_vec_.resize( model_num_hits_, 0.0 );
  posy_vec_.resize( model_num_hits_, 0.0 );
  posz_vec_.resize( model_num_hits_, 0.0 );
  num_array_.resize( model_num_hits_, 0 );
}


bool NNAnalyzer::Run(const std::vector<DetectorHit_sptr>& hits,
                     BasicComptonEvent_sptr& eventReconstructed)
{
  total_energy_deposit_ = 0.0;

  for (int i=0; i<model_num_hits_;i ++) {
    E_vec_[i]               = hits[i]->Energy();
    posx_vec_[i]            = hits[i]->PositionX();
    posy_vec_[i]            = hits[i]->PositionY();
    posz_vec_[i]            = hits[i]->PositionZ();
    total_energy_deposit_ += E_vec_[i];
  }

  std::vector<float> p(model_num_hits_);
  std::iota(p.begin(), p.end(), 0);
  sort(p.begin(), p.end(), [&](int a, int b) { return E_vec_[a] < E_vec_[b]; });

  std::vector<DetectorHit_sptr> ordered_hits(model_num_hits_);

  for (int i=0;i<model_num_hits_;i++) {
    ordered_hits[i]      = hits[p[i]];
    input_values_[i * numpar_per_hit_ + 0] = E_vec_[p[i]] / unit::keV;
    input_values_[i * numpar_per_hit_ + 1] = posx_vec_[p[i]] / unit::cm;
    input_values_[i * numpar_per_hit_ + 2] = posy_vec_[p[i]] / unit::cm;
    input_values_[i * numpar_per_hit_ + 3] = posz_vec_[p[i]] / unit::cm;
  }

  // Inference
  session_->Run(run_options_,
                input_names_char_.data(),  input_tensors_.data(), 1,
                output_names_char_.data(), outputTensors_.data(), 2);

  escape_flag_     = (output_tensor_escape_flag_values_[0] < output_tensor_escape_flag_values_[1]) ? true: false;
  pred_order_      = argmax(output_tensor_hit_order_values_.data(), num_order_combinations_);
  pred_order_prob_ = output_tensor_hit_order_values_[pred_order_];
  num_array_       = dict_label2array_[pred_order_];

  // OK up to here

  DetectorHit_sptr hit1_sptr = ordered_hits[num_array_[0]];
  DetectorHit_sptr hit2_sptr = ordered_hits[num_array_[1]];

  eventReconstructed->setHit1(0, hit1_sptr);
  eventReconstructed->setHit2(1, hit2_sptr);

  vector3_t hit1_pos(hit1_sptr->PositionX(), hit1_sptr->PositionY(), hit1_sptr->PositionZ());
  vector3_t hit2_pos(hit2_sptr->PositionX(), hit2_sptr->PositionY(), hit2_sptr->PositionZ());

  hit1_energy_     = input_values_[num_array_[0] * numpar_per_hit_ + 0] * unit::keV;
  hit2_energy_tmp_ = input_values_[num_array_[1] * numpar_per_hit_ + 0] * unit::keV;

  vector3_t init_scat_dir     = (hit2_pos - hit1_pos).unit();
  first_interaction_distance_ = (hit2_pos - hit1_pos).mag();

  // Energy estimation
  if ((escape_flag_==true) || (distinguish_escape_==false)) {
    vector3_t second_scat_dir_tmp(
      input_values_[num_array_[2] * numpar_per_hit_ + 1] - hit2_pos(0) / unit::cm,
      input_values_[num_array_[2] * numpar_per_hit_ + 2] - hit2_pos(1) / unit::cm,
      input_values_[num_array_[2] * numpar_per_hit_ + 3] - hit2_pos(2) / unit::cm);

    second_scat_cos_ = init_scat_dir.dot(second_scat_dir_tmp.unit());

    float hit3_energy_tmp = -0.5 * hit2_energy_tmp_
      + std::sqrt(0.25 * std::pow(hit2_energy_tmp_, 2.0)
                  + constant::electron_mass_c2 * hit2_energy_tmp_
                  / (1.0 - second_scat_cos_ + epsilon));

    hit2_energy_          = hit2_energy_tmp_ + hit3_energy_tmp;
    reconstructed_energy_ = hit1_energy_     + hit2_energy_;

    //Sum energy deposits
  }
  else{
    hit2_energy_          = total_energy_deposit_ - hit1_energy_;
    reconstructed_energy_ = total_energy_deposit_;
  }

  eventReconstructed->setHit2Energy(hit2_energy_);
  eventReconstructed->setNumberOfHits(model_num_hits_);
  eventReconstructed->setEscapeFlag(escape_flag_);
  eventReconstructed->setTotalEnergyDeposit(total_energy_deposit_);
  eventReconstructed->setLikelihood(pred_order_prob_);

  const double this_reconstruction_fraction = pred_order_prob_;
  eventReconstructed->setReconstructionFraction(this_reconstruction_fraction);

  return true;
}

std::pair<std::map<std::vector<int>, int>, std::map<int, std::vector<int>>>
label_dictionary_firstNhits(int num_hits, int first_n_hits_)
{
  std::vector<int> initial(num_hits);
  std::map<std::vector<int>, int> tuple_to_label;
  std::map<int, std::vector<int>> label_to_tuple;
  std::vector<std::vector<int>> unique_combinations;

  for (int i = 0; i < num_hits; ++i) {
    initial[i] = i;
  }

  do {
    std::vector<int> first_part(initial.begin(), initial.begin() + first_n_hits_);
    auto result = std::find(unique_combinations.begin(), unique_combinations.end(), first_part);
    if (result == unique_combinations.end()){
      unique_combinations.push_back(first_part);
    }
  } while (std::next_permutation(initial.begin(), initial.end()));

  int index = 0;
  for (const auto& combo: unique_combinations) {
    label_to_tuple[index] = combo;
    tuple_to_label[combo] = index;
    index++;
  }

  return {tuple_to_label, label_to_tuple};
}

int argmax(float *arg, int len)
{
  float max = -10000.0;
  int index_max = 0;
  do {
    if (arg[--len] > max) {
      max=arg[len];
      index_max = len;
    }
  } while(len > 0);
  return index_max;
}

// pretty prints a shape dimension std::vector
std::string print_shape(const std::vector<std::int64_t>& v)
{
  std::stringstream ss("");
  for (std::size_t i = 0; i < v.size() - 1; i++){
    ss << v[i] << "x";
  }
  ss << v[v.size() - 1];
  return ss.str();
}

template <typename T>
Ort::Value vec_to_tensor(std::vector<T>& data, const std::vector<std::int64_t>& shape)
{
  Ort::MemoryInfo mem_info =
    Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

  auto tensor = Ort::Value::CreateTensor<T>(mem_info, data.data(), data.size(), shape.data(), shape.size());
  return tensor;
}

int calculate_product(const std::vector<std::int64_t>& v)
{
  int total = 1;
  for (auto& i : v) { total *= i; }
  return total;
}

} /* namespace comptonsoft */
