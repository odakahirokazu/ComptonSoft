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

#ifndef COMPTONSOFT_MultiChannelData_H
#define COMPTONSOFT_MultiChannelData_H 1

#include <memory>
#include <vector>
#include <algorithm>
#include "CSTypes.hh"
#include "ChannelID.hh"
#include "VGainFunction.hh"

namespace comptonsoft {

/**
 * A class of data output from one ASIC.
 *
 * @author Hirokazu Odaka
 * @date 2007-12-xx
 * @date 2009-05-20 | change convertPHA2PI(), extrapolation calibration range
 * @date 2012-06-29 | simplification
 * @date 2014-09-07 | switch from array to vectors, introduce ElectrodeSide, change API.
 */
class MultiChannelData
{
public:
  MultiChannelData(std::size_t num_channels, ElectrodeSide eside);
  ~MultiChannelData();

  MultiChannelData(const MultiChannelData&) = default;
  MultiChannelData(MultiChannelData&&) = default;
  MultiChannelData& operator=(const MultiChannelData&) = default;
  MultiChannelData& operator=(MultiChannelData&&) = default;

  std::size_t NumberOfChannels() const { return NumChannels_; }
  ElectrodeSide getElectrodeSide() const { return ElectrodeSide_; }
  bool isAnodeSide() const
  { return ElectrodeSide_==ElectrodeSide::Anode; }
  bool isCathodeSide() const
  { return ElectrodeSide_==ElectrodeSide::Cathode; }

  void setReadoutSection(int readoutModuleID, int section)
  { return readoutID_.set(readoutModuleID, section, ChannelID::Undefined); }
  ReadoutChannelID ReadoutSection() const
  { return readoutID_; }

  bool isPrioritySide() const { return prioritySide_; }
  void setPrioritySide(bool v=true) { prioritySide_ = v; }

  void setUseNegativePulse(bool val)
  { useNegativePulse_ = val; }
  bool getUseNegativePulse() const
  { return useNegativePulse_; }

  void setThresholdEnergy(double val)
  { thresholdEnergy_ = val; }
  double getThresholdEnergy() const
  { return thresholdEnergy_; }

  void setNegativeThresholdEnergy(double val)
  { negativeThresholdEnergy_ = val; }
  double getNegativeThresholdEnergy() const
  { return negativeThresholdEnergy_; }
  
  void resetBadChannel()
  { std::fill(badChannels_.begin(), badChannels_.end(), 0); }
  void setBadChannel(std::size_t i, int val) { badChannels_[i] = val; }
  void setBadChannel(const std::vector<int8_t>& v) { badChannels_ = v; }
  int getBadChannel(std::size_t i) const { return badChannels_[i]; }
  void getBadChannelVector(std::vector<int>& v) const
  {
    v.resize(NumChannels_);
    std::copy(badChannels_.begin(), badChannels_.end(), v.begin());
  }
  bool getChannelEnable(std::size_t i) const { return badChannels_[i]==0; }

  void resetPedestal()
  { std::fill(pedestals_.begin(), pedestals_.end(), 0.0); }
  void setPedestal(std::size_t i, double val) { pedestals_[i] = val; }
  void setPedestal(const std::vector<double>& v) { pedestals_ = v; }
  double getPedestal(std::size_t i) const { return pedestals_[i]; }
  void getPedestalVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(pedestals_.begin(), pedestals_.end(), v.begin());
  }

  /**
   * register a gain function object. The receiver takes ownership of the given
   * object, so you should not delete it after calling this method.
   *
   * @param func a gain function object to register.
   */
  void registerGainFunction(std::unique_ptr<VGainFunction>&& func)
  { gainFunction_ = std::move(func); }
  void registerGainFunction(VGainFunction* func)
  { gainFunction_.reset(func); }
  
  void resetDataValid(int valid)
  { std::fill(dataValids_.begin(), dataValids_.end(), valid); }
  void setDataValid(std::size_t i, int8_t val) { dataValids_[i] = val; }
  void setDataValid(const std::vector<int8_t>& v) { dataValids_ = v; }
  double getDataValid(std::size_t i) const { return dataValids_[i]; }
  void getDataValidVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(dataValids_.begin(), dataValids_.end(), v.begin());
  }

  void resetRawADC()
  { std::fill(rawADCs_.begin(), rawADCs_.end(), 0.0); }
  void setRawADC(std::size_t i, int val) { rawADCs_[i] = val; }
  void setRawADC(const int* a, int n)
  { std::copy(a, a+n, rawADCs_.begin()); }
  void setRawADC(const short* a, int n)
  { std::copy(a, a+n, rawADCs_.begin()); }
  void setRawADC(const std::vector<int32_t>& v) { rawADCs_ = v; }
  int getRawADC(std::size_t i) const { return rawADCs_[i]; }
  void getRawADCVector(std::vector<int>& v) const
  {
    v.resize(NumChannels_);
    std::copy(rawADCs_.begin(), rawADCs_.end(), v.begin());
  }

  void resetPHA()
  { std::fill(PHAs_.begin(), PHAs_.end(), 0.0); }
  void setPHA(std::size_t i, double val) { PHAs_[i] = val; }
  void setPHA(const std::vector<double>& v) { PHAs_ = v; }
  double getPHA(std::size_t i) const { return PHAs_[i]; }
  void getPHAVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(PHAs_.begin(), PHAs_.end(), v.begin());
  }

  void resetEPI()
  { std::fill(EPIs_.begin(), EPIs_.end(), 0.0); }
  void setEPI(std::size_t i, double val) { EPIs_[i] = val; }
  void setEPI(const std::vector<double>& v) { EPIs_ = v; }
  double getEPI(std::size_t i) const { return EPIs_[i]; }
  void getEPIVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(EPIs_.begin(), EPIs_.end(), v.begin());
  }

  void resetHitChannel()
  { std::fill(hitChannels_.begin(), hitChannels_.end(), 0); }
  void setHitChannel(std::size_t i, int8_t val) { hitChannels_[i] = val; }
  void setHitChannel(const std::vector<int8_t> v) { hitChannels_ = v; }
  int getHitChannel(std::size_t i) const { return hitChannels_[i]; }
  void getHitChannelVector(std::vector<int>& v) const
  {
    v.resize(NumChannels_);
    std::copy(hitChannels_.begin(), hitChannels_.end(), v.begin());
  }
  
  void setCommonModeNoise(double v) { commonModeNoise_ = v; }
  double getCommonModeNoise() const { return commonModeNoise_; }

  void setReferenceLevel(double v) { referenceLevel_ = v; }
  double getReferenceLevel() const { return referenceLevel_; }

  /**
   * reset all event data (raw ADC, PHA, EPI, hit channels, common mode nose,
   * reference) in this object.
   * This method does not reset the data valid array.
   */
  void resetEventData()
  {
    resetRawADC();
    resetPHA();
    resetEPI();
    resetHitChannel();
    commonModeNoise_ = 0.0;
    referenceLevel_ = 0.0;
  }

  /**
   * copy Raw ADC values to the PHA array.
   */
  void copyToPHA()
  { std::copy(rawADCs_.begin(), rawADCs_.end(), PHAs_.begin()); }

  /**
   * correct pedestal levels on the PHA array.
   */
  void correctPedestalLevel();

  /**
   * calcualte common mode noise from the PHA array by the median method.
   * The calculated value is also set to _CommonModeNoise.
   * @return common mode noise.
   */
  double calculateCommonModeNoiseByMedian();

  /**
   * calcualte common mode noise from the PHA array by the mean method.
   * The calculated value is also set to _CommonModeNoise.
   * @return common mode noise.
   */
  double calculateCommonModeNoiseByMean();

  /**
   * subtract common mode noise on the PHA array.
   */
  void subtractCommonModeNoise();
 
  /**
   * convert PHA values to EPI values according to a calibaration curve for all
   * channels.
   * @return true if successful
   */
  bool convertPHA2EPI();

  /**
   * convert a pha value to a pi value according to a calibaration curve.
   * @param i channel ID
   * @param pha PHA value
   * @return EPI value
   */
  double PHA2EPI(std::size_t i, double pha) const;

  bool discriminate(double energy) const;

  /**
   * select detector readout channels that have higher EPI values than the
   * threshold energy.
   * The results can be accessed via @link getHitChannel(int) @endlink.
   */
  void selectHit();

private:
  const std::size_t NumChannels_;
  const ElectrodeSide ElectrodeSide_;
  
  ReadoutChannelID readoutID_;
  bool prioritySide_;
  bool useNegativePulse_;
  double thresholdEnergy_;
  double negativeThresholdEnergy_;

  std::vector<int8_t> badChannels_;
  std::vector<double> pedestals_;
  std::unique_ptr<VGainFunction> gainFunction_;

  std::vector<int8_t> dataValids_;
  std::vector<int32_t> rawADCs_;
  std::vector<double> PHAs_;
  std::vector<double> EPIs_;
  std::vector<int8_t> hitChannels_;
  double commonModeNoise_;
  double referenceLevel_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_MultiChannelData_H */
