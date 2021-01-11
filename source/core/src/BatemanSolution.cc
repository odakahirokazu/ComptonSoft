/**
 * @file BatemanSolution.cc
 * @brief source file of class BatemanSolution
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#include "BatemanSolution.hh"

#include <iterator>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <boost/format.hpp>
#include "G4SystemOfUnits.hh"

#include "RIDecayChains.hh"


namespace {

comptonsoft::longer_float_t get_sum(const std::vector<comptonsoft::longer_float_t>& v)
{
  // to be reviewed

  using float_type = comptonsoft::longer_float_t;
  std::vector<float_type> pv;
  std::vector<float_type> nv;
  for (float_type a: v) {
    if (a < 0.0) { nv.push_back(a); }
    else { pv.push_back(a); }
  }
  std::sort(std::begin(pv), std::end(pv),
            [](float_type a, float_type b)->bool { return (a < b); });
  std::sort(std::begin(nv), std::end(nv),
            [](float_type a, float_type b)->bool { return (b < a); });
  float_type psum = std::accumulate(std::begin(pv), std::end(pv), static_cast<float_type>(0));
  float_type nsum = std::accumulate(std::begin(nv), std::end(nv), static_cast<float_type>(0));
  float_type sum = psum+nsum;
  return sum;
}

void modify_lambda_if_same_values(std::vector<comptonsoft::longer_float_t>& xs,
                                  std::size_t first_index,
                                  std::size_t last_index,
                                  const comptonsoft::RIDecayChain& chain)
{
  constexpr comptonsoft::longer_float_t epsilon = 1.0e-3;
  bool changed = false;
  for (std::size_t i=first_index; i<=last_index; i++) {
    for (std::size_t j=i+1; j<=last_index; j++) {
      if (std::abs(xs[j]-xs[i])/xs[i] < epsilon) {
        const double factor = (xs[j]>=xs[i]) ? (1.0+2.0*epsilon) : (1.0/(1.0+2.0*epsilon));
        const comptonsoft::longer_float_t l_modified = xs[j]*factor;
        {
          const comptonsoft::IsotopeInfo isotope = chain[j-1].Isotope();
          std::cout << "lambda modified at " << j << " : "
                    << "(" << isotope.Z() << "," << isotope.A() << ") : "
                    << xs[j]*CLHEP::s << " -> " << l_modified*CLHEP::s << std::endl;
        }
        xs[j] = l_modified;
        changed = true;
      }
    }
  }

  if (changed) {
    modify_lambda_if_same_values(xs, first_index, last_index, chain);
  }
}

} /* anonymous namespace */

namespace comptonsoft
{

BatemanSolution::BatemanSolution()
  : verbose_level_(0),
    threshold_lambda_(1.0e-24l)
{
}

BatemanSolution::~BatemanSolution() = default;

namespace {

}

void BatemanSolution::solve(const RIDecayChain& chain)
{
  const std::size_t N = chain.size();

  lambda_.assign(N+1, 0.0l);
  for (std::size_t i=1; i<=N-1; i++) {
    lambda_[i] = chain[i].DecayConstant();
  }

  modify_lambda_if_same_values(lambda_, 1, N-1, chain);

  D_.resize(N);
  for (auto& Di: D_) { Di.assign(N+1, 0.0l); }
  for (std::size_t i=1; i<=N-1; i++) {
    for (std::size_t j=1; j<=i+1; j++) {
      longer_float_t d(1.0);
      for (std::size_t p=1; p<=i+1; p++) {
        const longer_float_t Ap = (p==i+1) ? 1.0l : lambda_[p];
        const longer_float_t Bp = (p==j) ? 1.0l : 1.0l/(lambda_[p] - lambda_[j]);
        d *= (Ap*Bp);
      }
      D_[i][j] = d;
      
      if (verbose_level_ >= 8) {
        std::cout << boost::format("D[%d][%d] = %.9e") % i % j % D_[i][j] << std::endl;
      }
    }
  }
}

double BatemanSolution::getSolution(std::size_t i, double t) const
{
  using std::exp;
  using std::abs;

  if (t<0.0l) { return 0.0l; }

  longer_float_t s = 0.0l;
  if (i==0) {
    const longer_float_t lambda = lambda_[1];
    s = exp(-lambda*t);
  }
  else {
    std::vector<longer_float_t> sj(i+1, 0.0l);
    for (std::size_t j=1; j<=i+1; j++) {
      const longer_float_t lambda = lambda_[j];
      sj[j-1] = D_[i][j] * exp(-lambda*t);
    }
    s = get_sum(sj);
  }
  return s;
}

double BatemanSolution::getConvolution(std::size_t i,
                                       double tau1,
                                       double tau2,
                                       double t) const
{
  using std::exp;
  using std::expm1;
  using std::abs;

  const longer_float_t dt1 = (t>tau1) ? (t-tau1) : 0.0l;
  const longer_float_t dt2 = (t>tau2) ? (t-tau2) : 0.0l;

  longer_float_t s = 0.0l;
  if (i==0) {
    const longer_float_t lambda = lambda_[1];
    if (lambda < ThresholdLambda()) {
      s = dt1 - dt2;
    }
    else {
      const longer_float_t y = -lambda*(dt2-dt1);
      if (abs(y) < 1.0l) {
        s = exp(-lambda*dt1)*expm1(y)/lambda;
      }
      else {
        s = (exp(-lambda*dt2)-exp(-lambda*dt1))/lambda;
      }
    }
  }
  else {
    std::vector<longer_float_t> sj(i+1, 0.0l);
    for (std::size_t j=1; j<=i+1; j++) {
      const longer_float_t lambda = lambda_[j];
      longer_float_t st = 0.0l;
      if (lambda < ThresholdLambda()) {
        st = D_[i][j] * (dt1 - dt2);
      }
      else {
        const longer_float_t y = -lambda*(dt2-dt1);
        if (abs(y) < 1.0l) {
          st = D_[i][j]*exp(-lambda*dt1)*expm1(-lambda*(dt2-dt1))/lambda;
        }
        else {
          st = D_[i][j]*(exp(-lambda*dt2)-exp(-lambda*dt1))/lambda;
        }
      }
      sj[j-1] = st;
    }
    s = get_sum(sj);
  }
  return s;
}

double BatemanSolution::getIntegralConvolution(std::size_t i,
                                               double tau1,
                                               double tau2,
                                               double t1,
                                               double t2) const
{
  // we assume: tau1 <= tau2 and t1 <= t2
  if (tau1>tau2 || t1>t2) {
    return 0.0l;
  }

  if (t2<=tau1) { // observation is before irradiation (t1, t2, tau1, tau2)
    return 0.0l;
  }
  else if (tau2<=t1) { // no overlap (tau1, tau2, t1, t2)
    return getIntegralConvolution1(i, tau1, tau2, t1, t2);
  }
  else if (t2<=tau2) {
    if (t1<tau1) { // (t1, tau1, t2, tau2)
      return getIntegralConvolution2(i, tau1, t2, tau1);
    }
    else { // (tau1, t1, t2, tau2)
      return getIntegralConvolution2(i, tau1, t2, t1);
    }
  }
  else {
    if (t1<tau1) { // (t1, tau1, tau2, t2)
      const longer_float_t s1 = getIntegralConvolution2(i, tau1, tau2, tau1);
      const longer_float_t s2 = getIntegralConvolution1(i, tau1, tau2, tau2, t2);
      return s1+s2;
    }
    else { // (tau1, t1, tau2, t2)
      const longer_float_t s1 = getIntegralConvolution2(i, tau1, tau2, t1);
      const longer_float_t s2 = getIntegralConvolution1(i, tau1, tau2, tau2, t2);
      return s1+s2;
    }
  }
  
  return 0.0l;
}

double BatemanSolution::getIntegralConvolution1(std::size_t i,
                                                double tau1,
                                                double tau2,
                                                double t1,
                                                double t2) const
{
  using std::exp;
  using std::expm1;
  using std::abs;

  const longer_float_t dt11 = t1 - tau1;
  const longer_float_t dt12 = t1 - tau2;
  const longer_float_t dt21 = t2 - tau1;
  const longer_float_t dt22 = t2 - tau2;

  longer_float_t s = 0.0l;
  if (i==0) {
    const longer_float_t lambda = lambda_[1];
    if (lambda < ThresholdLambda()) {
      s = (tau2 - tau1) * (t2 - t1);
    }
    else {
      longer_float_t s1(0.0l), s2(0.0l);
      const longer_float_t y1 = -lambda*(dt21-dt11);
      if (abs(y1) < 1.0l) {
        s1 = exp(-lambda*dt11)*expm1(y1);
      }
      else {
        s1 = exp(-lambda*dt21)-exp(-lambda*dt11);
      }
      const longer_float_t y2 = -lambda*(dt12-dt22);
      if (abs(y1) < 1.0l) {
        s2 = exp(-lambda*dt22)*expm1(y2);
      }
      else {
        s2 = exp(-lambda*dt12)-exp(-lambda*dt22);
      }
      s = (s1+s2)/lambda/lambda;
    }
  }
  else {
    std::vector<longer_float_t> sj(i+1, 0.0l);
    for (std::size_t j=1; j<=i+1; j++) {
      const longer_float_t lambda = lambda_[j];
      longer_float_t st = 0.0l;
      if (lambda < ThresholdLambda()) {
        st = D_[i][j] * (tau2 - tau1) * (t2 - t1);
      }
      else {
        longer_float_t s1(0.0l), s2(0.0l);
        const longer_float_t y1 = -lambda*(dt21-dt11);
        if (abs(y1) < 1.0l) {
          s1 = exp(-lambda*dt11)*expm1(y1);
        }
        else {
          s1 = exp(-lambda*dt21)-exp(-lambda*dt11);
        }
        const longer_float_t y2 = -lambda*(dt12-dt22);
        if (abs(y1) < 1.0l) {
          s2 = exp(-lambda*dt22)*expm1(y2);
        }
        else {
          s2 = exp(-lambda*dt12)-exp(-lambda*dt22);
        }
        st = D_[i][j]*(s1+s2)/lambda/lambda;
      }
      sj[j-1] = st;
    }
    s = get_sum(sj);
  }
  return s;
}

double BatemanSolution::getIntegralConvolution2(std::size_t i,
                                                double tau1,
                                                double tau2,
                                                double t1) const
{
  using std::exp;
  using std::expm1;
  using std::abs;

  const longer_float_t t2 = tau2; // re-assignment of t2
  const longer_float_t dt11 = t1 - tau1;
  const longer_float_t dt21 = t2 - tau1;

  longer_float_t s = 0.0l;
  if (i==0) {
    const longer_float_t lambda = lambda_[1];
    if (lambda < ThresholdLambda()) {
      s = 0.5 * (dt21*dt21 - dt11*dt11);
    }
    else {
      longer_float_t s1(0.0l);
      const longer_float_t y = -lambda*(dt21-dt11);
      if (abs(y) < 1.0l) {
        s1 = exp(-lambda*dt11)*expm1(y);
      }
      else {
        s1 = exp(-lambda*dt21)-exp(-lambda*dt11);
      }
      s = (lambda*(t2-t1)+s1)/lambda/lambda;
    }
  }
  else {
    std::vector<longer_float_t> sj(i+1, 0.0l);
    for (std::size_t j=1; j<=i+1; j++) {
      const longer_float_t lambda = lambda_[j];
      longer_float_t st = 0.0l;
      if (lambda < ThresholdLambda()) {
        st = D_[i][j] * 0.5 * (dt21*dt21 - dt11*dt11);
      }
      else {
        longer_float_t s1(0.0l);
        const longer_float_t y = -lambda*(dt21-dt11);
        if (abs(y) < 1.0l) {
          s1 = exp(-lambda*dt11)*expm1(y);
        }
        else {
          s1 = exp(-lambda*dt21)-exp(-lambda*dt11);
        }
        st = D_[i][j]*(lambda*(t2-t1)+s1)/lambda/lambda;
      }
      sj[j-1] = st;
    }
    s = get_sum(sj);
  }
  return s;
}

} /* namespace comptonsoft */
