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
 * @file RIDecayChains.hh
 * @brief header file of class RIDecayChains
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#ifndef COMPTONSOFT_RIDecayChains_H
#define COMPTONSOFT_RIDecayChains_H 1

#include <vector>
#include <list>
#include <map>
#include <cstdint>
#include "IsotopeInfo.hh"
#include "TimeProfile.hh"
#include "RIDecayProperties.hh"
#include "BatemanSolution.hh"

class G4ParticleDefinition;
class G4Ions;
class G4RadioactiveDecay;

namespace comptonsoft
{

double invert_lifetime(double lifetime);


class RIDecayChains
{
public:
  explicit RIDecayChains(const IsotopeInfo& isotope);
  virtual ~RIDecayChains();

  RIDecayChains(const RIDecayChains&) = delete;
  RIDecayChains(RIDecayChains&&) = delete;
  RIDecayChains& operator=(const RIDecayChains&) = delete;
  RIDecayChains& operator=(RIDecayChains&&) = delete;

  void setVerboseLevel(int v) { verbose_level_ = v; }

  void setDecayConstantThreshold(double v)
  { decay_constant_threshold_ = v; }
  double DecayConstantThreshold() const
  { return decay_constant_threshold_; }

  void setLifetimeThreshold(double v)
  { lifetime_threshold_ = v; }
  double LifetimeThreshold() const
  { return lifetime_threshold_; }

  void setBranchingRatioThreshold(double v)
  { branching_ratio_threshold_ = v; }
  double BranchingRatioThreshold() const
  { return branching_ratio_threshold_; }

  void setDecayProcess(G4RadioactiveDecay* process)
  { decay_process_ = process; }

  void build();

  std::size_t NumberOfChains() const { return chains_.size(); }
  const RIDecayChain getDecayChain(std::size_t i) const
  { return chains_[i]; }

  void prepareSolutions();
  void solve(double t);
  void solve(const TimeProfile& timeProfile, double t);
  void solve(const TimeProfile& timeProfile, double t1, double t2);
  void solve(const TimeProfile& timeProfile,
             const std::vector<std::pair<double, double>>& measurementWindows);

  const std::map<int64_t, double>& getResults() const
  { return total_counts_; }

  void printChains() const;
  void printTotalCounts() const;
  
private:
  void buildChain(const IsotopeInfo& parentIsotope, int depth);
  std::list<RIDecayProperties> collectDecayProducts(const G4Ions* parent);
  std::list<RIDecayProperties> collectInternalTransitionProducts(const G4Ions* parent);

  void forceInstantaneousDecay(std::list<RIDecayProperties>& decayProducts);
  void compressDecayProducts(std::list<RIDecayProperties>& decayProducts);

  void solveChain(const BatemanSolution& solution,
                  double t,
                  RIDecayChain& chain);
  void solveChain(const BatemanSolution& solution,
                  const TimeProfile& timeProfile,
                  double t,
                  RIDecayChain& chain);
  void solveChain(const BatemanSolution& solution,
                  const TimeProfile& timeProfile,
                  double t1,
                  double t2,
                  bool is_rate_additive,
                  RIDecayChain& chain);

  void takeAverage(double totalTime);
  void makeTotal();

private:
  int verbose_level_ = 0;
  bool use_photon_evaporation_data_ = true;
  double decay_constant_threshold_ = 0.0;
  double lifetime_threshold_ = 0.0;
  double branching_ratio_threshold_ = 0.0;
  double low_energy_limit_ = 0.0;
  IsotopeInfo isotope_;
  std::vector<RIDecayChain> chains_;
  std::vector<BatemanSolution> solutions_;
  std::map<int64_t, double> total_counts_;
  G4RadioactiveDecay* decay_process_ = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RIDecayChains_H */
