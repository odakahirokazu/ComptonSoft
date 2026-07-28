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

#ifndef COMPTONSOFT_CalculatePedestalLevels_H
#define COMPTONSOFT_CalculatePedestalLevels_H 1

#include "VCSModule.hh"
#include "TH1.h"

namespace comptonsoft {

class CalculatePedestalLevels : public VCSModule
{
  DEFINE_ANL_MODULE(CalculatePedestalLevels, 2.2);
public:
  CalculatePedestalLevels();
  ~CalculatePedestalLevels() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

private:
  std::vector<TH1*> spectrum_histograms_;
  std::vector<TH1*> pedestral_histograms_;
  std::vector<TH1*> noise_histograms_;

  int num_bins_ = 1024;
  double ADC_min_;
  double ADC_max_;
  bool negative_ADC_ = false;

  int ADC_zero_level_bin_ = 250;
  int peak_search_half_width_ = 245;
  int average_range_half_width_ = 8;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CalculatePedestalLevels_H */
