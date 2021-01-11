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
 * @file RIDecayProperties.hh
 * @brief header file of class RIDecayProperties
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#ifndef COMPTONSOFT_RIDecayProperties_H
#define COMPTONSOFT_RIDecayProperties_H 1

#include <vector>
#include "IsotopeInfo.hh"

namespace comptonsoft
{

class RIDecayProperties
{
public:
  RIDecayProperties(double decay_constant,
                    double branching_ratio,
                    IsotopeInfo isotope);
  RIDecayProperties(double decay_constant,
                    double branching_ratio,
                    IsotopeInfo&& isotope);

  ~RIDecayProperties();

  RIDecayProperties(const RIDecayProperties&) = default;
  RIDecayProperties(RIDecayProperties&&) = default;
  RIDecayProperties& operator=(const RIDecayProperties&) = default;
  RIDecayProperties& operator=(RIDecayProperties&&) = default;

  void setDecayConstant(double v) { decay_constant_ = v; }
  double DecayConstant() const { return decay_constant_; }

  void setBranchingRatio(double v) { branching_ratio_ = v; }
  double BranchingRatio() const { return branching_ratio_; }

  void setIsotope(IsotopeInfo v) { isotope_ = v; }
  IsotopeInfo Isotope() const { return isotope_; }

  void setRate(double v) { isotope_.setRate(v); }
  void addRate(double v) { isotope_.addRate(v); }

  void setMerged(bool v) { merged_ = v; }
  bool isMerged() const { return merged_; }

private:
  // properties of decay
  double decay_constant_;
  double branching_ratio_;

  // properties of the daughter
  IsotopeInfo isotope_;
  bool merged_;
};

using RIDecayChain = std::vector<RIDecayProperties>;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RIDecayProperties_H */
