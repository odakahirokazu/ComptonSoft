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

#include "OnnxInference.hh"

namespace comptonsoft {

OnnxInference::OnnxInference(const std::string& model_path)
  : env_(ORT_LOGGING_LEVEL_WARNING, "comptonsoft"),
    session_options_()
{
  // Basic runtime configuration
  session_options_.SetIntraOpNumThreads(1);
  session_options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);

  // Load ONNX model
  session_ = std::make_unique<Ort::Session>(env_, model_path.c_str(), session_options_);

  Ort::AllocatorWithDefaultOptions allocator;

  const size_t num_inputs  = session_->GetInputCount();
  const size_t num_outputs = session_->GetOutputCount();

  input_names_.reserve(num_inputs);
  output_names_.reserve(num_outputs);

  // Retrieve input names (copied into std::string for ownership)
  for (size_t i = 0; i < num_inputs; ++i) {
    Ort::AllocatedStringPtr name_ptr = session_->GetInputNameAllocated(i, allocator);
    input_names_.emplace_back(name_ptr.get());
    // name_ptr frees the internal buffer when going out of scope
  }

  // Retrieve output names
  for (size_t i = 0; i < num_outputs; ++i) {
    Ort::AllocatedStringPtr name_ptr = session_->GetOutputNameAllocated(i, allocator);
    output_names_.emplace_back(name_ptr.get());
  }
}

std::vector<float> OnnxInference::infer(const std::vector<float>& input)
{
  // Prepare CPU memory info
  Ort::MemoryInfo mem_info = Ort::MemoryInfo::CreateCpu(
    OrtDeviceAllocator, OrtMemTypeCPU);

  // Here we assume a 1D tensor [N]
  std::vector<int64_t> input_shape = {
    static_cast<int64_t>(input.size())
  };

  // Create input tensor
  Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
    mem_info,
    const_cast<float*>(input.data()),
    input.size(),
    input_shape.data(),
    input_shape.size()
    );

  // Build local const-char* arrays for ONNX Runtime
  // (Rebuilt every call for safety; overhead is negligible)
  std::vector<const char*> input_names_cstr;
  input_names_cstr.reserve(input_names_.size());
  for (const auto& s : input_names_) {
    input_names_cstr.push_back(s.c_str());
  }

  std::vector<const char*> output_names_cstr;
  output_names_cstr.reserve(output_names_.size());
  for (const auto& s : output_names_) {
    output_names_cstr.push_back(s.c_str());
  }

  // Run inference
  auto output_tensors = session_->Run(
    Ort::RunOptions{nullptr},
    input_names_cstr.data(),
    &input_tensor,
    1,  // one input tensor
    output_names_cstr.data(),
    output_names_cstr.size()
    );

  // Here we assume the first output is a 1D float tensor
  Ort::Value& out = output_tensors.front();

  float* output_data = out.GetTensorMutableData<float>();
  const size_t output_size =
    out.GetTensorTypeAndShapeInfo().GetElementCount();

  return std::vector<float>(output_data, output_data + output_size);
}

} // namespace comptonsoft
