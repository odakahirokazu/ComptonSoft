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

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <onnxruntime_cxx_api.h>

namespace comptonsoft {

/**
 * class for a neutral network model inference using ONNX Runtime
 *
 * @author Satoshi Takashima, Hirokazu Odaka
 * @date 2025-11-25
 */
class OnnxInference {
public:
  /**
   * Constructor: loads the ONNX model and initializes ONNX Runtime
   */
  explicit OnnxInference(const std::string& model_path);

  ~OnnxInference() = default;

  OnnxInference(const OnnxInference&) = delete;
  OnnxInference& operator=(const OnnxInference&) = delete;

  OnnxInference(OnnxInference&&) = default;
  OnnxInference& operator=(OnnxInference&&) = default;

  std::vector<std::vector<float>> infer(const std::vector<float>& input);

private:
  Ort::Env env_;
  Ort::SessionOptions session_options_;
  std::unique_ptr<Ort::Session> session_;

  std::vector<std::string> input_names_;
  std::vector<std::string> output_names_;
};

} // namespace comptonsoft
