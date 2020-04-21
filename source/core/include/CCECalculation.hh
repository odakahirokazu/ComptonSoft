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

#ifndef COMPTONSOFT_CCECalculation_H
#define COMPTONSOFT_CCECalculation_H 1

#include <memory>
#include <vector>
#include "CSTypes.hh"
#include "VGoalRegion.hh"
#include "SemiconductorModel.hh"

namespace comptonsoft {

/**
 * CCE calcultion class
 *
 * @author Hiromasa Suzuki, Hirokazu Odaka
 * @date 2020-04-18
 */
class CCECalculation
{
public:
  CCECalculation();
  virtual ~CCECalculation();

  void setUpsideReadout(bool v);
  void setPixelDimensions(double x, double y, double z);
  
  void setSegmentation(const std::vector<double>& xs,
                       const std::vector<double>& ys,
                       const std::vector<double>& zs);
  void setStepLength(double v) { stepLength_ = v; }
  void setEndingTime(double v) { endingTime_ = v; }
  void registerGoal(std::unique_ptr<VGoalRegion>&& goal) { goal_ = std::move(goal); }
  void initializeCCEMap(const std::string& filename);

  void setSemiconductorProperties(bool carrier_positive,
                                  double mobility,
                                  double lifetime,
                                  double temperature,
                                  double spreadFactor);
  void setCarrierVelocitySaturation(double efield);
  void unsetCarrierVelocitySaturation();
  void loadEField(const std::string& filename);
  void useAccurateWeightingPotential();
  void setRandomSeed(uint32_t seed);

  void run(int numParticles);
  void write();

private:
  std::vector<double> extendedEdges(const std::vector<double>& xs,
                                    double pixelWidth);
  void calculate(int ix, int iy, int iz, int numParticles);
  void transport(int ix, int iy, int iz, double charge);
  void calculateForOppositeSide(int ix, int iy, int iz);
  void addCCE(int binx, int biny, int binz, double value);

private:
  double pixelWidthX_ = 0.0;
  double pixelWidthY_ = 0.0;
  double thickness_ = 0.0;
  std::vector<double> segmentationX_;
  std::vector<double> segmentationY_;
  std::vector<double> segmentationZ_;
  double stepLength_ = 0.0;
  double endingTime_ = 0.0;
  std::unique_ptr<VGoalRegion> goal_;
  std::unique_ptr<SemiconductorModel> field_;
  TH3D* cceMap_ = nullptr;
  std::unique_ptr<TFile> outFile_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CCECalculation_H */
