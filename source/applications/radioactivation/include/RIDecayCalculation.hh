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

/**
 * @file RIDecayCalculation.hh
 * @brief header file of class RIDecayCalculation
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#ifndef COMPTONSOFT_RIDecayCalculation_H
#define COMPTONSOFT_RIDecayCalculation_H 1

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "RateData.hh"

class G4RunManager;

namespace comptonsoft {

class TimeProfile;

class RIDecayCalculation
{
public:
  RIDecayCalculation();
  virtual ~RIDecayCalculation();

  RIDecayCalculation(const RIDecayCalculation&) = delete;
  RIDecayCalculation(RIDecayCalculation&&) = delete;
  RIDecayCalculation& operator=(const RIDecayCalculation&) = delete;
  RIDecayCalculation& operator=(RIDecayCalculation&&) = delete;

  void setVerboseLevel(int v) { verbose_level_ = v; }

  void setInputFiles(const std::string& production_rate,
                     const std::string& irradiation)
  {
    filename_production_rate_ = production_rate;
    filename_irradiation_ = irradiation;
  }

  void setMeasurementTime(double t);
  double MeasurememtTime() const;

  void addMeasurementWindow(double t1, double t2);
  void clearMeasurememtWindows() { measurement_windows_.clear(); }
  double getStartingTimeOfWindow(std::size_t i) const;
  double getEndingTimeOfWindow(std::size_t i) const;
  
  void setCountThreshold(double v) { count_threshold_ = v; }
  double CountThreshold() const { return count_threshold_; }
  
  void setDecayConstantThreshold(double v);
  double DecayConstantThreshold() const;

  void setBranchingRatioThreshold(double v)
  { branching_ratio_threshold_ = v; }
  double BranchingRatioThreshold() const
  { return branching_ratio_threshold_; }

  void setOutputFile(const std::string& decay_rate)
  {
    filename_decay_rate_ = decay_rate;
  }
  
  bool initialize();
  void perform();
  void output();

protected:
  void initialize_geant4();

private:
  RateVector makeDecayRateVector(const std::map<int64_t, double>& accumulation);
  
private:
  int verbose_level_ = 0;
  
  // input files
  std::string filename_production_rate_;
  std::string filename_irradiation_;

  bool average_mode_ = false;
  double measurement_time_ = 0.0;
  std::vector<std::pair<double, double>> measurement_windows_;

  double count_threshold_ = 0.0;
  double decay_constant_threshold_ = 0.0;
  double branching_ratio_threshold_ = 0.0;

  // output file
  std::string filename_decay_rate_;

  std::unique_ptr<G4RunManager> geant4_run_manager_;
  std::unique_ptr<TimeProfile> irradiation_profile_;
  std::unique_ptr<RateData> production_rate_data_;

  std::unique_ptr<RateData> decay_rate_data_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RIDecayCalculation_H */
