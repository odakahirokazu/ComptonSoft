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

#ifndef COMPTONSOFT_OneASICData_H
#define COMPTONSOFT_OneASICData_H 1

#include <vector>
#include <algorithm>

class TSpline;

namespace comptonsoft {


/**
 * A class of data output from one ASIC.
 * @author Hirokazu Odaka
 * @date 2007-12-xx
 * @date 2008-09-03
 * @date 2009-05-20
 * @date 2009-06-02
 * @date 2011-06-06
 * @date 2011-09-10
 * @date 2012-06-29 | simplification
 */
class OneASICData
{
public:
  OneASICData(int n, bool anode);
  OneASICData(const OneASICData& r);
  virtual ~OneASICData();

  int NumChannel() const { return m_NumChannel; }
  bool isAnodeSide() const { return m_AnodeSide; }
  bool isCathodeSide() const { return !isAnodeSide(); }
  bool isNSide() const { return isAnodeSide(); }
  bool isPSide() const { return !isNSide(); }

  bool isPrioritySide() const { return m_PrioritySide; }
  void setPrioritySide(bool v=true) { m_PrioritySide = v; }

  void resetRawADC()
  { std::fill(m_RawADC, m_RawADC+m_NumChannel, 0.0); }
  void setRawADC(int ch, int val)
  { m_RawADC[ch] = val; }
  void setRawADC(const int* a, int n)
  { std::copy(a, a+n, m_RawADC); }
  void setRawADC(const short* a, int n)
  { std::copy(a, a+n, m_RawADC); }
  void setRawADC(const std::vector<int>& v)
  { std::copy(v.begin(), v.end(), m_RawADC); }
  int getRawADC(int ch) const
  { return m_RawADC[ch]; }
  void getRawADCArray(int* a, int n) const
  { std::copy(m_RawADC, m_RawADC+n, a); }
  void getRawADCVector(std::vector<int>& v) const
  {
    if (v.size() < static_cast<unsigned int>(m_NumChannel)) {
      v.resize(m_NumChannel);
    }
    std::copy(m_RawADC, m_RawADC+m_NumChannel, v.begin());
  }

  void resetBadChannel()
  { std::fill(m_BadChannel, m_BadChannel+m_NumChannel, 0); }
  void setBadChannel(int ch, int val)
  { m_BadChannel[ch] = val; }
  void setBadChannel(const int* a, int n)
  { std::copy(a, a+n, m_BadChannel); }
  void setBadChannel(const std::vector<int>& v)
  { std::copy(v.begin(), v.end(), m_BadChannel); }
  int getBadChannel(int ch) const
  { return m_BadChannel[ch]; }
  void getBadChannelArray(int* a, int n) const
  { std::copy(m_BadChannel, m_BadChannel+n, a); }
  void getBadChannelVector(std::vector<int>& v) const
  {
    if (v.size() < static_cast<unsigned int>(m_NumChannel)) {
      v.resize(m_NumChannel);
    }
    std::copy(m_BadChannel, m_BadChannel+m_NumChannel, v.begin());
  }

  void resetPedestal()
  { std::fill(m_Pedestal, m_Pedestal+m_NumChannel, 0); }
  void setPedestal(int ch, double val)
  { m_Pedestal[ch] = val; }
  void setPedestal(const double* a, int n)
  { std::copy(a, a+n, m_Pedestal); }
  void setPedestal(const std::vector<double>& v)
  { std::copy(v.begin(), v.end(), m_Pedestal); }
  double getPedestal(int ch) const
  { return m_Pedestal[ch]; }
  void getPedestalArray(double* a, int n) const
  { std::copy(m_Pedestal, m_Pedestal+n, a); }
  void getPedestalVector(std::vector<double>& v) const
  {
    if (v.size() < static_cast<unsigned int>(m_NumChannel)) {
      v.resize(m_NumChannel);
    }
    std::copy(m_Pedestal, m_Pedestal+m_NumChannel, v.begin());
  }

  void resetGainFactor()
  { std::fill(m_GainFactor, m_GainFactor+m_NumChannel, 1.0); }
  void setGainFactor(int ch, double val)
  { m_GainFactor[ch] = val; }
  void setGainFactor(const double* a, int n)
  { std::copy(a, a+n, m_GainFactor); }
  void setGainFactor(const std::vector<double>& v)
  { std::copy(v.begin(), v.end(), m_GainFactor); }
  double getGainFactor(int ch) const
  { return m_GainFactor[ch]; }
  void getGainFactorArray(double* a, int n) const
  { std::copy(m_GainFactor, m_GainFactor+n, a); }
  void getGainFactorVector(std::vector<double>& v) const
  {
    if (v.size() < static_cast<unsigned int>(m_NumChannel)) {
      v.resize(m_NumChannel);
    }
    std::copy(m_GainFactor, m_GainFactor+m_NumChannel, v.begin());
  }

  virtual void resetCalSpline() {}
  virtual void registerCalSpline(int , const TSpline* ) {}
  virtual void registerCalSpline(const TSpline** , int ) {}
  virtual void registerCalSpline(const std::vector<const TSpline*>& ) {}
  
  void resetPHA()
  { std::fill(m_PHA, m_PHA+m_NumChannel, 0.0); }
  void setPHA(int ch, double val)
  { m_PHA[ch] = val; }
  void setPHA(const double* a, int n)
  { std::copy(a, a+n, m_PHA); }
  void setPHA(const std::vector<double>& v)
  { std::copy(v.begin(), v.end(), m_PHA); }
  double getPHA(int ch) const { return m_PHA[ch]; }
  void getPHAArray(double* a, int n) const
  { std::copy(m_PHA, m_PHA+n, a); }
  void getPHAVector(std::vector<double>& v) const
  {
    if (v.size() < static_cast<unsigned int>(m_NumChannel)) {
      v.resize(m_NumChannel);
    }
    std::copy(m_PHA, m_PHA+m_NumChannel, v.begin());
  }

  void resetPI()
  { std::fill(m_PI, m_PI+m_NumChannel, 0.0); }
  void setPI(int ch, double val)
  { m_PI[ch] = val; }
  void setPI(const double* a, int n)
  { std::copy(a, a+n, m_PI); }
  void setPI(const std::vector<double>& v)
  { std::copy(v.begin(), v.end(), m_PI); }
  double getPI(int ch) const { return m_PI[ch]; }
  void getPIArray(double* a, int n) const
  { std::copy(m_PI, m_PI+n, a); }
  void getPIVector(std::vector<double>& v) const
  {
    if (v.size() < static_cast<unsigned int>(m_NumChannel)) {
      v.resize(m_NumChannel);
    }
    std::copy(m_PI, m_PI+m_NumChannel, v.begin());
  }

  void resetHitChannel()
  { std::fill(m_HitChannel, m_HitChannel+m_NumChannel, 0); }
  int getHitChannel(int ch) const { return m_HitChannel[ch]; }
  void getHitChannelArray(int* a, int n) const
  { std::copy(m_HitChannel, m_HitChannel+n, a); }
  void getHitChannelVector(std::vector<int>& v) const
  {
    if (v.size() < static_cast<unsigned int>(m_NumChannel)) {
      v.resize(m_NumChannel);
    }
    std::copy(m_HitChannel, m_HitChannel+m_NumChannel, v.begin());
  }
  
  void setThresholdEnergy(double val) { m_ThresholdEnergy = val; }
  double getThresholdEnergy() const { return m_ThresholdEnergy; }
  
  void setCommonModeNoise(double v) { m_CommonModeNoise = v; }
  double getCommonModeNoise() const { return m_CommonModeNoise; }
  double calculateCommonModeNoiseByMedian();
  double calculateCommonModeNoiseByMean();

  void setReference(double v) { m_Reference = v; }
  double getReference() { return m_Reference; }
 
  /**
   * convert PHA values to PI values according to a calibaration curve for all channels.
   * @return true if successful
   */
  virtual bool convertPHA2PI();

  /**
   * convert a pha value to a pi value according to a calibaration curve.
   * @param ch channel ID
   * @param pha PHA value
   * @return PI value
   */
  virtual double PHA2PI(int ch, double pha) const;

  /**
   * select detector readout channels which have higher PI values than the threshold energy.
   * The results can be accessed via @link getHitChannel(int) @endlink
   */
  virtual void selectHit();

private:
  const int m_NumChannel;
  const bool m_AnodeSide;

  static const int ERR_VALUE = -1000000000;

  bool m_PrioritySide;
  int* m_RawADC;
  int* m_BadChannel;
  double* m_Pedestal;
  // const TSpline **m_CalSpline;
  double* m_GainFactor;
  double* m_PHA;
  double* m_PI;
  int* m_HitChannel;

  double m_ThresholdEnergy;

  double m_CommonModeNoise;
  double m_Reference;

private:
  OneASICData& operator=(const OneASICData& r);
};

}

#endif /* COMPTONSOFT_OneASICData_H */
