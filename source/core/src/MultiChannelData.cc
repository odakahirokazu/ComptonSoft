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

#include "MultiChannelData.hh"
#include "AstroUnits.hh"
#include "GainFunctionLinear.hh"

namespace comptonsoft {

MultiChannelData::MultiChannelData(std::size_t n, ElectrodeSide eside)
  : NumChannels_(n),
    ElectrodeSide_(eside),
    prioritySide_(true),
    useNegativePulse_(false),
    thresholdEnergy_(0.0),
    negativeThresholdEnergy_(0.0),
    badChannels_(n, 0),
    pedestals_(n, 0.0),
    gainFunction_(new GainFunctionLinear(n, 1000.0)),
    dataValids_(n, 1),
    rawADCs_(n, 0),
    PHAs_(n, 0.0),
    EPIs_(n, 0.0),
    hitChannels_(n, 0),
    commonModeNoise_(0.0),
    referenceLevel_(0.0)
{
}

MultiChannelData::~MultiChannelData() = default;

void MultiChannelData::correctPedestalLevel()
{
  const std::size_t N = NumChannels_;
  for (std::size_t i=0; i<N; i++) {
    if (getDataValid(i) && getChannelEnable(i)) {
      PHAs_[i] -= pedestals_[i];
    }
  }
}

double MultiChannelData::calculateCommonModeNoiseByMedian()
{
  const std::size_t N = NumChannels_;
  std::vector<double> sortedPHA;
  for (std::size_t i=0; i<N; i++) {
    if (getDataValid(i) && getChannelEnable(i)) {
      sortedPHA.push_back(getPHA(i));
    }
  }
  std::size_t numGoodChannel = sortedPHA.size();
  if (numGoodChannel < 1) { return 0.0; }

  std::sort(sortedPHA.begin(), sortedPHA.end());
  double median = sortedPHA[numGoodChannel/2];
  commonModeNoise_ = median;
  return median;
}

double MultiChannelData::calculateCommonModeNoiseByMean()
{
  const std::size_t N = NumChannels_;
  std::vector<double> adc(N);
  double max1(-1.0e9), max2(-1.0e9), max3(-1.0e9);
  double min1(+1.0e9), min2(+1.0e9), min3(+1.0e9);

  double sum = 0.0;
  int numGoodChannel = 0;
  
  for (std::size_t i=0; i<N; i++) {
    if (getDataValid(i) && getChannelEnable(i)) {
      double pha = getPHA(i);
      sum += pha;
      numGoodChannel++;
      
      if (max3 < pha) {
        max3 = pha;
        if (max2 < max3) {
          double t1 = max2; max2 = max3; max3 = t1; // swap
          if (max1 < max2) {
            double t2 = max1; max1 = max2; max2 = t2; // swap
          }
        }
      }
      
      if (min3 > pha) {
        min3 = pha;
        if (min2 > min3) {
          double t1 = min2; min2 = min3; min3 = t1; // swap
          if (min1 > min2) {
            double t2 = min1; min1 = min2; min2 = t2; // swap
          }
        }
      }
    }
  }

  double mean = 0.0;
  if (numGoodChannel > 6) {
    mean = (sum-max1-max2-max3-min1-min2-min3)/(numGoodChannel-6);
  }
  commonModeNoise_ = mean;
  return mean;
}

void MultiChannelData::subtractCommonModeNoise()
{
  const std::size_t N = NumChannels_;
  const double CMN = commonModeNoise_;
  for (std::size_t i=0; i<N; i++) {
    if (getDataValid(i) && getChannelEnable(i)) {
      PHAs_[i] -= CMN;
    }
  }
}

double MultiChannelData::PHA2EPI(std::size_t i, double pha) const
{
  double vpi = 0.0;
  const double RangeMin = gainFunction_->RangeMin(i);
  const double RangeMax = gainFunction_->RangeMax(i);
  if (pha < RangeMin) {
    vpi = gainFunction_->eval(i, pha);
  }
  else if (RangeMax < pha) {
    vpi = gainFunction_->eval(i, pha);
  }
  else {
    vpi = gainFunction_->eval(i, pha);
  }
  return vpi*keV;
}

bool MultiChannelData::convertPHA2EPI()
{
  const std::size_t N = NumChannels_;
  for (std::size_t i=0; i<N; i++) {
    if (getDataValid(i) && getChannelEnable(i)) {
      setEPI(i, PHA2EPI(i, getPHA(i)));
    }
    else {
      setEPI(i, 0.0);
    }
  }
  return true;
}

bool MultiChannelData::discriminate(const double energy) const
{
  if (energy > getThresholdEnergy()) {
    return true;
  }
  else if (getUseNegativePulse() && energy<getNegativeThresholdEnergy()) {
    return true;
  }
  return false;
}

void MultiChannelData::selectHit()
{
  const std::size_t N = NumChannels_;
  for (std::size_t i=0; i<N; i++) {
    if (getDataValid(i) && getChannelEnable(i) && discriminate(EPIs_[i])) {
      hitChannels_[i] = 1;
    }
    else {
      hitChannels_[i] = 0;
    }
  }
}

} /* namespace comptonsoft */
