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
 * @file BatemanSolution.hh
 * @brief header file of class BatemanSolution
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#ifndef COMPTONSOFT_BatemanSolution_H
#define COMPTONSOFT_BatemanSolution_H 1

#include <vector>
#include "RIDecayProperties.hh"

namespace comptonsoft
{

using longer_float_t = long double;

class BatemanSolution
{
public:
  BatemanSolution();
  virtual ~BatemanSolution();
  BatemanSolution(const BatemanSolution&) = default;
  BatemanSolution(BatemanSolution&&) = default;
  BatemanSolution& operator=(const BatemanSolution&) = default;
  BatemanSolution& operator=(BatemanSolution&&) = default;

  void setVerboseLevel(int v) { verbose_level_ = v; }
  double VerboseLevel() const { return verbose_level_; }

  void setThresholdLambda(double v) { threshold_lambda_ = v; }
  double ThresholdLambda() const { return threshold_lambda_; }

  void solve(const RIDecayChain& chain);
  double getSolution(std::size_t i, double t) const;
  double getConvolution(std::size_t i,
                        double tau1, double tau2, double t) const;
  double getIntegralConvolution(std::size_t i,
                                double tau1, double tau2,
                                double t1, double t2) const;
  double getIntegralConvolution1(std::size_t i,
                                 double tau1, double tau2,
                                 double t1, double t2) const;
  double getIntegralConvolution2(std::size_t i,
                                 double tau1, double tau2,
                                 double t1) const;

private:
  int verbose_level_ = 0;
  std::vector<longer_float_t> lambda_;
  std::vector<std::vector<longer_float_t>> D_;
  longer_float_t threshold_lambda_ = 0.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_BatemanSolution_H */
