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

#ifndef COMPTONSOFT_NNEstimator_H
#define COMPTONSOFT_NNEstimator_H 1

#include "VEventReconstructionAlgorithm.hh"
#include "DetectorHit.hh"
#include "DetectorHit_sptr.hh"
#include <string>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <memory>
//#include <onnxruntime/onnxruntime_cxx_api.h>
#include <onnxruntime_cxx_api.h>
#include <CLHEP/Vector/ThreeVector.h>

#define PI 3.141592653
#define MASS_ELECTRON_KEV 510.99895

namespace comptonsoft {

using std::map;
using std::string;
using std::vector;
using std::pair;

pair< map<vector<int>, int>, map<int, vector<int> > > 
label_dictionary_firstNhits(int num_hits, int first_n_hits_);

int argmax(float *arg, int len);

int calculate_product(const std::vector<std::int64_t>& v);

std::string print_shape(const std::vector<std::int64_t>& v);

template <typename T>
Ort::Value vec_to_tensor(std::vector<T>& data, const std::vector<std::int64_t>& shape);


class NNAnalyzer
{
public:
    NNAnalyzer(const char* trained_model_path, int model_num_hits);
    ~NNAnalyzer();
    void SetupModel(bool);
    bool Run(const std::vector<DetectorHit_sptr>& hits, 
                BasicComptonEvent_sptr& eventReconstructed);


private:
    int model_num_hits_;
    string trained_model_path_str_;
    map< int, vector<int> > dict_label2array_;

    Ort::Env* env_;
    Ort::MemoryInfo* memory_info_;
    Ort::SessionOptions* session_options_;
    Ort::RunOptions run_options_;
    Ort::AllocatorWithDefaultOptions* alloc_;
    Ort::Session* session_;

    vector<Ort::AllocatedStringPtr> inputNames_;
    vector<Ort::AllocatedStringPtr> outputNames_;

    vector<std::string>  input_names_;
    vector<std::int64_t> input_shape_;
    vector<std::string>  output_names_;

    vector<float>   output_escape_flag_values_;
    vector<int64_t> output_escape_flag_shape_;
    vector<int64_t> output_shapes_escape_flag_;

    vector<float>   output_hit_order_values_;
    vector<int64_t> output_hit_order_shape_;
    vector<int64_t> output_shapes_hit_order_;

    vector<const char*> input_names_char_;
    vector<const char*> output_names_char_;

    vector<Ort::Value> input_tensors_;
    Ort::Value outputTensor_hit_order_;
    Ort::Value outputTensor_escape_flag_;
    vector<Ort::Value> outputTensors_;

    int dim_input_;
    vector<float> input_values_;
    vector<float> output_tensor_hit_order_values_;
    vector<float> output_tensor_escape_flag_values_;

    vector<float> E_vec_;
    vector<float> posx_vec_;
    vector<float> posy_vec_;
    vector<float> posz_vec_;

    float hit1_energy_;
    float hit2_energy_;
    float hit2_energy_tmp_;

    bool distinguish_escape_;
    bool escape_flag_;

    int num_order_combinations_;

    const float epsilon       = 1e-9;
    const int numpar_per_hit_ = 4;
    const int first_n_hits_   = 3;

    vector<int> num_array_;

    double total_energy_deposit_;
    float  reconstructed_energy_;
    float second_scat_cos_;
    float first_interaction_distance_;

    int pred_order_;
    float pred_order_prob_; 
};


} /* namespace comptonsoft */

#endif /* COMPTONSOFT_NNEstimator_H */
