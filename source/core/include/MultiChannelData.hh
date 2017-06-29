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

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <algorithm>
#include "CSTypes.hh"
#include "FlagDefinition.hh"
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
 * @date 2016-05-02 | PHA randomization
 * @date 2016-08-19 | threshold: scalar value to vector
 * @date 2016-11-11 | add time and flags
 */
class MultiChannelData
{
public:
  MultiChannelData(std::size_t num_channels, ElectrodeSide eside);
  virtual ~MultiChannelData();

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

  void setReadoutID(int readoutModuleID, int section)
  { return readoutID_.set(readoutModuleID, section, ChannelID::Undefined); }
  ReadoutBasedChannelID ReadoutID() const
  { return readoutID_; }

  bool isPrioritySide() const { return prioritySide_; }
  void setPrioritySide(bool v=true) { prioritySide_ = v; }

  void setUseNegativePulse(bool val)
  { useNegativePulse_ = val; }
  bool getUseNegativePulse() const
  { return useNegativePulse_; }

  void resetThresholdEnergyVector(double val)
  { std::fill(thresholdEnergyVector_.begin(), thresholdEnergyVector_.end(), val); }
  void setThresholdEnergy(std::size_t i, double val) { thresholdEnergyVector_[i] = val; }
  void setThresholdEnergyVector(const std::vector<double>& v) { thresholdEnergyVector_ = v; }
  double getThresholdEnergy(std::size_t i) const { return thresholdEnergyVector_[i]; }
  void getThresholdEnergyVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(thresholdEnergyVector_.begin(), thresholdEnergyVector_.end(), v.begin());
  }

  void resetNegativeThresholdEnergyVector(double val)
  { std::fill(negativeThresholdEnergyVector_.begin(), negativeThresholdEnergyVector_.end(), val); }
  void setNegativeThresholdEnergy(std::size_t i, double val) { negativeThresholdEnergyVector_[i] = val; }
  void setNegativeThresholdEnergyVector(const std::vector<double>& v) { negativeThresholdEnergyVector_ = v; }
  double getNegativeThresholdEnergy(std::size_t i) const { return negativeThresholdEnergyVector_[i]; }
  void getNegativeThresholdEnergyVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(negativeThresholdEnergyVector_.begin(), negativeThresholdEnergyVector_.end(), v.begin());
  }

  void setTime(double v) { time_ = v; }
  double Time() const { return time_; }

  void setFlags(uint64_t v) { flags_ = v; }
  uint64_t Flags() const { return flags_; }

  void resetChannelDisabledVector(int v=0)
  { std::fill(channelDisabledVector_.begin(), channelDisabledVector_.end(), v); }
  void setChannelDisabled(std::size_t i, int val) { channelDisabledVector_[i] = val; }
  void setChannelDisabledVector(const std::vector<int8_t>& v) { channelDisabledVector_ = v; }
  int getChannelDisabled(std::size_t i) const { return channelDisabledVector_[i]; }
  void getChannelDisabledVector(std::vector<int>& v) const
  {
    v.resize(NumChannels_);
    std::copy(channelDisabledVector_.begin(), channelDisabledVector_.end(), v.begin());
  }
  bool getChannelEnabled(std::size_t i) const
  {
    const int status = channelDisabledVector_[i];
    return (status==channel_status::normal) || (status==channel_status::trigger_disable);
  }

  void resetPedestalVector()
  { std::fill(pedestalVector_.begin(), pedestalVector_.end(), 0.0); }
  void setPedestal(std::size_t i, double val) { pedestalVector_[i] = val; }
  void setPedestalVector(const std::vector<double>& v) { pedestalVector_ = v; }
  double getPedestal(std::size_t i) const { return pedestalVector_[i]; }
  void getPedestalVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(pedestalVector_.begin(), pedestalVector_.end(), v.begin());
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
  
  void resetDataValidVector(int valid)
  { std::fill(dataValidVector_.begin(), dataValidVector_.end(), valid); }
  void setDataValid(std::size_t i, int8_t val) { dataValidVector_[i] = val; }
  void setDataValidVector(const std::vector<int8_t>& v) { dataValidVector_ = v; }
  double getDataValid(std::size_t i) const { return dataValidVector_[i]; }
  void getDataValidVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(dataValidVector_.begin(), dataValidVector_.end(), v.begin());
  }

  void resetRawADCVector()
  { std::fill(rawADCVector_.begin(), rawADCVector_.end(), 0.0); }
  void setRawADC(std::size_t i, int val) { rawADCVector_[i] = val; }
  void setRawADC(const int* a, int n)
  { std::copy(a, a+n, rawADCVector_.begin()); }
  void setRawADC(const short* a, int n)
  { std::copy(a, a+n, rawADCVector_.begin()); }
  void setRawADCVector(const std::vector<int32_t>& v) { rawADCVector_ = v; }
  int getRawADC(std::size_t i) const { return rawADCVector_[i]; }
  void getRawADCVector(std::vector<int>& v) const
  {
    v.resize(NumChannels_);
    std::copy(rawADCVector_.begin(), rawADCVector_.end(), v.begin());
  }

  void resetPHAVector()
  { std::fill(PHAVector_.begin(), PHAVector_.end(), 0.0); }
  void setPHA(std::size_t i, double val) { PHAVector_[i] = val; }
  void setPHAVector(const std::vector<double>& v) { PHAVector_ = v; }
  double getPHA(std::size_t i) const { return PHAVector_[i]; }
  void getPHAVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(PHAVector_.begin(), PHAVector_.end(), v.begin());
  }

  void resetEPIVector()
  { std::fill(EPIVector_.begin(), EPIVector_.end(), 0.0); }
  void setEPI(std::size_t i, double val) { EPIVector_[i] = val; }
  void setEPIVector(const std::vector<double>& v) { EPIVector_ = v; }
  double getEPI(std::size_t i) const { return EPIVector_[i]; }
  void getEPIVector(std::vector<double>& v) const
  {
    v.resize(NumChannels_);
    std::copy(EPIVector_.begin(), EPIVector_.end(), v.begin());
  }

  void resetChannelHitVector()
  { std::fill(channelHitVector_.begin(), channelHitVector_.end(), 0); }
  void setChannelHit(std::size_t i, int8_t val) { channelHitVector_[i] = val; }
  void setChannelHitVector(const std::vector<int8_t> v) { channelHitVector_ = v; }
  int getChannelHit(std::size_t i) const { return channelHitVector_[i]; }
  void getChannelHitVector(std::vector<int>& v) const
  {
    v.resize(NumChannels_);
    std::copy(channelHitVector_.begin(), channelHitVector_.end(), v.begin());
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
    time_ = 0.0;
    flags_ = 0u;
    resetRawADCVector();
    resetPHAVector();
    resetEPIVector();
    resetChannelHitVector();
    commonModeNoise_ = 0.0;
    referenceLevel_ = 0.0;
  }

  /**
   * copy Raw ADC values to the PHA array.
   */
  void copyToPHA()
  { std::copy(rawADCVector_.begin(), rawADCVector_.end(), PHAVector_.begin()); }

  /**
   * randomize each PHA value in its bin to prevent discreteness.
   */
  void randomizePHAValues();

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

  /**
   * select detector readout channels that have higher EPI values than the
   * threshold energy.
   * The results can be accessed via @link getHitChannel(int) @endlink.
   */
  void selectHits();

  bool discriminate(std::size_t i, double energy) const;

private:
  const std::size_t NumChannels_;
  const ElectrodeSide ElectrodeSide_;
  
  ReadoutBasedChannelID readoutID_;
  bool prioritySide_;
  bool useNegativePulse_;
  std::vector<double> thresholdEnergyVector_;
  std::vector<double> negativeThresholdEnergyVector_;

  double time_ = 0.0;
  uint64_t flags_ = 0u;

  std::vector<int8_t> channelDisabledVector_;
  std::vector<double> pedestalVector_;
  std::unique_ptr<VGainFunction> gainFunction_;

  std::vector<int8_t> dataValidVector_;
  std::vector<int32_t> rawADCVector_;
  std::vector<double> PHAVector_;
  std::vector<double> EPIVector_;
  std::vector<int8_t> channelHitVector_;
  double commonModeNoise_;
  double referenceLevel_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_MultiChannelData_H */
