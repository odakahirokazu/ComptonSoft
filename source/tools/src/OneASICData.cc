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

// OneASICData.cc
// 2007-12-xx Hirokazu Odaka
// 2008-09-03 Hirokazu Odaka
// 2009-05-20 Hirokazu Odaka : change convertPHA2PI(), extrapolation calibration range
// 2009-06-02 Hirokazu Odaka : convertPHA2PI()
// 2011-06-06 Hirokazu Odaka : add m_Reference

#include "OneASICData.hh"

#include "globals.hh"
// #include "TSpline.h"

namespace comptonsoft {

OneASICData::OneASICData(int n, bool anode)
  : m_NumChannel(n), m_AnodeSide(anode), m_PrioritySide(true)
{
  const int nch = m_NumChannel;
  
  m_RawADC = new int[nch];
  m_BadChannel = new int[nch];
  m_Pedestal = new double[nch];
  m_GainFactor = new double[nch];
  m_PHA = new double[nch];
  m_PI = new double[nch];
  m_HitChannel = new int[nch];

  m_ThresholdEnergy = 10.0 * keV;

  m_CommonModeNoise = 0.0;
  m_Reference = 0.0;

  resetRawADC();
  resetBadChannel();
  resetPedestal();
  resetGainFactor();
  resetPHA();
  resetPI();
  resetHitChannel();
}


OneASICData::OneASICData(const OneASICData& r)
  : m_NumChannel(r.m_NumChannel),
    m_AnodeSide(r.m_AnodeSide)
{
  const int nch = m_NumChannel;
  m_PrioritySide = r.m_PrioritySide;

  m_RawADC = new int[nch];
  m_BadChannel = new int[nch];
  m_Pedestal = new double[nch];
  m_GainFactor = new double[nch];
  m_PHA = new double[nch];
  m_PI = new double[nch];
  m_HitChannel = new int[nch];

  m_ThresholdEnergy = r.m_ThresholdEnergy;
  
  m_CommonModeNoise = r.m_CommonModeNoise;

  setRawADC(r.m_RawADC, nch);
  setBadChannel(r.m_BadChannel, nch);
  setPedestal(r.m_Pedestal, nch);
  setGainFactor(r.m_GainFactor, nch);
  setPHA(r.m_PHA, nch);
  setPI(r.m_PI, nch);
  std::copy(r.m_HitChannel, r.m_HitChannel+nch, m_HitChannel);
}


OneASICData::~OneASICData()
{
  delete[] m_HitChannel;
  delete[] m_PI;
  delete[] m_PHA;
  delete[] m_GainFactor;
  delete[] m_Pedestal;
  delete[] m_BadChannel;
  delete[] m_RawADC;
}


double OneASICData::calculateCommonModeNoiseByMedian()
{
  const int nCH = m_NumChannel;
  std::vector<double> adc(nCH);
  std::vector<double> adcSort;
 
  for (int i=0; i<nCH; i++) {
    adc[i] = static_cast<double>(m_RawADC[i]) - m_Pedestal[i];
    if (m_BadChannel[i] == 0) {
      adcSort.push_back(adc[i]);
    }
  }

  int numGoodChannel = adcSort.size();
  if (numGoodChannel < 1) { return 0.0; }

  sort(adcSort.begin(), adcSort.end());
  double median = adcSort[numGoodChannel/2];

  for (int i=0; i<nCH; i++) {
    m_PHA[i] = adc[i] - median;
  }

  m_CommonModeNoise = median;
  return median;
}


double OneASICData::calculateCommonModeNoiseByMean()
{
  const int nCH = m_NumChannel;
  std::vector<double> adc(nCH);
  double max1=-1000000000.0, max2=-1000000000.0, max3=-1000000000.0;
  double min1=+1000000000.0, min2=+1000000000.0, min3=+1000000000.0;
  
  for (int i=0; i<nCH; i++) {
    adc[i] = static_cast<double>(m_RawADC[i]) - m_Pedestal[i];
  }

  double mean = 0.;
  int numGoodChannel = 0;

  for (int i=0; i<nCH; i++) {
    if (m_BadChannel[i] == 1) {
      continue;
    }

    double adctmp = adc[i];
    mean += adctmp;
    numGoodChannel++;
    
    if (max3 < adctmp) {
      max3 = adctmp;
      if (max2 < max3) {
        double t1 = max2;
        max2 = max3;
        max3 = t1;
	if (max1 < max2) {
          double t2 = max1;
          max1 = max2;
          max2 = t2;
	}
      }
    }

    if (min3 > adctmp) {
      min3 = adctmp;
      if (min2 > min3) {
        double t1 = min2;
        min2 = min3;
        min3 = t1;
        if (min1 > min2) {
          double t2 = min1;
          min1 = min2;
          min2 = t2;
        }
      }
    }
  }

  if (numGoodChannel > 6) {
    mean = (mean-max1-max2-max3-min1-min2-min3)/(numGoodChannel-6);
  }
  else {
    mean = 0.0;
  }
  
  for (int i=0; i<nCH; i++) {
    m_PHA[i] = adc[i] - mean;
  }

  m_CommonModeNoise = mean;
  return mean;
}


double OneASICData::PHA2PI(int ch, double pha) const
{
  double gain = m_GainFactor[ch];
  return pha * gain * keV;
}


bool OneASICData::convertPHA2PI()
{
  const int nCH = m_NumChannel;
  for (int i=0; i<nCH; i++) {
    if (m_BadChannel[i] == 0) {
      m_PI[i] = m_PHA[i] * m_GainFactor[i] * keV;
    }
  }
  return true;
}


void OneASICData::selectHit()
{
  const int nCH = m_NumChannel;
  const double thr = m_ThresholdEnergy;
  for (int i=0; i<nCH; i++) {
    if (m_BadChannel[i] == 0) {
      if (m_PI[i] > thr) {
        m_HitChannel[i] = 1;
      }
      else {
        m_HitChannel[i] = 0;
      }
    }
    else {
      m_HitChannel[i] = 0;
    }
  }
}
	
}
