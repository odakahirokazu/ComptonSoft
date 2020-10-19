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

#ifndef COMPTONSOFT_VDeviceSimulation_H
#define COMPTONSOFT_VDeviceSimulation_H 1

#include <tuple>
#include <vector>
#include <list>
#include <iostream>
#include "DetectorHit_sptr.hh"
#include "VDetectorUnit.hh"
#include "PixelID.hh"

namespace comptonsoft {

/**
 * A virtual class for simulating a semiconductor detector device.
 * This class will provide methods to convert a deposited energy of a hit into a detector output such as an electrical pulse height, taking into account of a detector response.
 * Since the class also provides interface to such functions, you can derive a new specific class from this class to realize special treatment of detector response. 
 *
 * @author Hirokazu Odaka
 * @date 2007-xx-xx | DeviceSimulation
 * @date 2008-07-29
 * @date 2009-10-26 
 * @date 2012-07-01 | VDeviceSimulation 
 * @date 2014-10-08 | VDeviceSimulation 
 * @date 2020-09-02 | treat EPI as a tuple of its value and error
 */
class VDeviceSimulation : virtual public VDetectorUnit
{
public:
  VDeviceSimulation();
  virtual ~VDeviceSimulation();

public:
  virtual void initializeEvent();

  void setDepthSensingMode(int mode) { DepthSensingMode_ = mode; }
  void setDepthResolution(double v) { DepthResolution_ = v; }
  int DepthSensingMode() const { return DepthSensingMode_; }
  double DepthResolution() const { return DepthResolution_; }
  
  void setQuenchingFactor(double c1, double c2=0.0, double c3=0.0)
  { QuenchingFactor_ = std::make_tuple(c1, c2, c3); }
  double QuenchingFactor() const { return QuenchingFactor1(); }
  double QuenchingFactor1() const { return std::get<0>(QuenchingFactor_); }
  double QuenchingFactor2() const { return std::get<1>(QuenchingFactor_); }
  double QuenchingFactor3() const { return std::get<2>(QuenchingFactor_); }

  template <typename IndexType>
  void setChannelDisabled(IndexType i, int v)
  { setTableValue(&VDeviceSimulation::ChannelDisabledVector_, i, v); }

  template <typename SelectorType>
  void setChannelDisabledToSelected(int v, SelectorType selector)
  { setTableValueToSelected(&VDeviceSimulation::ChannelDisabledVector_, v, selector); }

  template <typename IndexType>
  int getChannelDisabled(IndexType i) const
  { return getTableValue(&VDeviceSimulation::ChannelDisabledVector_, i); }

  void resetChannelDisabledVector(int v)
  { resetTable(&VDeviceSimulation::ChannelDisabledVector_, v); }

  template <typename IndexType>
  void setNoiseParam0(IndexType i, double v)
  { setTableValue(&VDeviceSimulation::NoiseParam0Vector_, i, v); }

  template <typename SelectorType>
  void setNoiseParam0ToSelected(double v, SelectorType selector)
  { setTableValueToSelected(&VDeviceSimulation::NoiseParam0Vector_, v, selector); }

  template <typename IndexType>
  double getNoiseParam0(IndexType i) const
  { return getTableValue(&VDeviceSimulation::NoiseParam0Vector_, i); }

  void resetNoiseParam0Vector(double v)
  { resetTable(&VDeviceSimulation::NoiseParam0Vector_, v); }

  template <typename IndexType>
  void setNoiseParam1(IndexType i, double v)
  { setTableValue(&VDeviceSimulation::NoiseParam1Vector_, i, v); }

  template <typename SelectorType>
  void setNoiseParam1ToSelected(double v, SelectorType selector)
  { setTableValueToSelected(&VDeviceSimulation::NoiseParam1Vector_, v, selector); }

  template <typename IndexType>
  double getNoiseParam1(IndexType i) const
  { return getTableValue(&VDeviceSimulation::NoiseParam1Vector_, i); }

  void resetNoiseParam1Vector(double v)
  { resetTable(&VDeviceSimulation::NoiseParam1Vector_, v); }

  template <typename IndexType>
  void setNoiseParam2(IndexType i, double v)
  { setTableValue(&VDeviceSimulation::NoiseParam2Vector_, i, v); }

  template <typename SelectorType>
  void setNoiseParam2ToSelected(double v, SelectorType selector)
  { setTableValueToSelected(&VDeviceSimulation::NoiseParam2Vector_, v, selector); }

  template <typename IndexType>
  double getNoiseParam2(IndexType i) const
  { return getTableValue(&VDeviceSimulation::NoiseParam2Vector_, i); }

  void resetNoiseParam2Vector(double v)
  { resetTable(&VDeviceSimulation::NoiseParam2Vector_, v); }

  template <typename IndexType>
  void setThreshold(IndexType i, double v)
  { setTableValue(&VDeviceSimulation::ThresholdVector_, i, v); }

  template <typename SelectorType>
  void setThresholdToSelected(double v, SelectorType selector)
  { setTableValueToSelected(&VDeviceSimulation::ThresholdVector_, v, selector); }

  template <typename IndexType>
  double getThreshold(IndexType i) const
  { return getTableValue(&VDeviceSimulation::ThresholdVector_, i); }

  void resetThresholdVector(double v)
  { resetTable(&VDeviceSimulation::ThresholdVector_, v); }

  template <typename IndexType>
  void setTriggerDiscriminationCenter(IndexType i, double v)
  { setTableValue(&VDeviceSimulation::TriggerDiscriminationCenterVector_, i, v); }

  template <typename SelectorType>
  void setTriggerDiscriminationCenterToSelected(double v, SelectorType selector)
  { setTableValueToSelected(&VDeviceSimulation::TriggerDiscriminationCenterVector_, v, selector); }

  template <typename IndexType>
  double getTriggerDiscriminationCenter(IndexType i) const
  { return getTableValue(&VDeviceSimulation::TriggerDiscriminationCenterVector_, i); }

  void resetTriggerDiscriminationCenterVector(double v)
  { resetTable(&VDeviceSimulation::TriggerDiscriminationCenterVector_, v); }

  template <typename IndexType>
  void setTriggerDiscriminationSigma(IndexType i, double v)
  { setTableValue(&VDeviceSimulation::TriggerDiscriminationSigmaVector_, i, v); }

  template <typename SelectorType>
  void setTriggerDiscriminationSigmaToSelected(double v, SelectorType selector)
  { setTableValueToSelected(&VDeviceSimulation::TriggerDiscriminationSigmaVector_, v, selector); }

  template <typename IndexType>
  double getTriggerDiscriminationSigma(IndexType i) const
  { return getTableValue(&VDeviceSimulation::TriggerDiscriminationSigmaVector_, i); }

  void resetTriggerDiscriminationSigmaVector(double v)
  { resetTable(&VDeviceSimulation::TriggerDiscriminationSigmaVector_, v); }

  template <typename IndexType>
  PixelID getPixelID(IndexType i) const
  { return getTableValue(&VDeviceSimulation::PixelIDVector_, i); }

  virtual void initializeTables() = 0;

  void setTimingResolutionForTrigger(double val) { TimeResolutionFast_ = val; }
  double TimingResolutionForTrigger() const { return TimeResolutionFast_; }
  void setTimingResolutionForEnergyMeasurement(double val) { TimeResolutionSlow_ = val; }
  double TimingResolutionForEnergyMeasurement() const { return TimeResolutionSlow_; }

  void enablePedestal(bool b=true) { pedestalEnabled_ = b; }
  bool isPedestalEnabled() const { return pedestalEnabled_; }

  void makeDetectorHits();
  void makeRawDetectorHits();

  void prepareForTimingProcess();
  bool isSelfTriggered() const;
  double FirstTriggerTime() const;
  void makeDetectorHitsAtTime(double time_triggered, int time_group);

  virtual void applyQuenching(DetectorHit_sptr hit) const;
  virtual double calculateEnergyCharge(const PixelID& pixel,
                                       double energyDeposit,
                                       double x, double y, double z) const = 0;
  virtual std::tuple<double, double> calculateEPI(double energyCharge, const PixelID& pixel) const = 0;
  void makeEPI(DetectorHit_sptr hit);

  bool checkTriggerDiscrimination(double energyCharge, const PixelID& pixel) const;

  void fillPixel(DetectorHit_sptr hit) const;

  virtual void printSimulationParameters(std::ostream& os) const = 0;
  
  void insertRawHit(DetectorHit_sptr hit) { RawHits_.push_back(hit); }

  void assignLocalDepth(DetectorHit_sptr hit) const;
  void assignLocalPositionError(DetectorHit_sptr hit) const;

protected:
  virtual bool checkRange(const PixelID& pixel) const = 0;
  virtual int IndexOfTable(const PixelID& pixel) const = 0;
  virtual int IndexOfTable(const SectionChannelPair& channel) const
  { return IndexOfTable(ChannelToPixel(channel)); }
  virtual int SizeOfTable() const = 0;
  virtual PixelID TableIndexToPixelID(int index) const = 0;

  template <typename IndexType>
  void setPixelID(IndexType i, PixelID v)
  { setTableValue(&VDeviceSimulation::PixelIDVector_, i, v); }

  int NumberOfRawHits() const { return RawHits_.size(); }
  DetectorHit_sptr getRawHit(int i) { return RawHits_[i]; }

  void insertSimulatedHit(DetectorHit_sptr hit)
  { SimulatedHits_.push_back(hit); }

private:
  virtual void simulatePulseHeights() = 0;
  
  void sortHitsInTimeOrder(std::list<DetectorHit_sptr>& hits);

  void removeHitsOutOfPixelRange(std::list<DetectorHit_sptr>& hits);
  void removeHitsAtChannelsDisabled(std::list<DetectorHit_sptr>& hits);
  void removeHitsBelowThresholds(std::list<DetectorHit_sptr>& hits);
  void mergeHits(std::list<DetectorHit_sptr>& hits);
  void mergeHitsIfCoincident(double time_width,
                             std::list<DetectorHit_sptr>& hits);

  void performTriggerDiscrimination();

  std::list<DetectorHit_sptr> generatePedestalSignals(int time_group,
                                                      double time_of_signal) const;

protected:
  template <typename ObjectType, typename ValueType, typename IndexType>
  void setTableValue(std::vector<ValueType> ObjectType::*table,
                     IndexType index, ValueType v)
  {
    (static_cast<ObjectType*>(this)->*table)[IndexOfTable(index)] = v;
  }

  template <typename ObjectType, typename ValueType, typename SelectorType>
  void setTableValueToSelected(std::vector<ValueType> ObjectType::*table,
                               ValueType v, SelectorType selector)
  {
    const int N = SizeOfTable();
    for (int i=0; i<N; i++) {
      PixelID pixel = PixelIDVector_[i];
      if (selector(pixel)) {
        (static_cast<ObjectType*>(this)->*table)[i] = v;
      }
    }
  }

  template <typename ObjectType, typename ValueType>
  void resetTable(std::vector<ValueType> ObjectType::*table,
                  ValueType v)
  {
    (static_cast<ObjectType*>(this)->*table).assign(SizeOfTable(), v);
  }

  template <typename ObjectType, typename ValueType, typename IndexType>
  ValueType getTableValue(std::vector<ValueType> ObjectType::*table,
                          IndexType index) const
  {
    return (static_cast<const ObjectType*>(this)->*table)[IndexOfTable(index)];
  }

private:
  int DepthSensingMode_;
  double DepthResolution_;
  std::tuple<double, double, double> QuenchingFactor_;

  // channels properties
  std::vector<PixelID> PixelIDVector_;
  std::vector<int> ChannelDisabledVector_;
  std::vector<double> NoiseParam0Vector_;
  std::vector<double> NoiseParam1Vector_;
  std::vector<double> NoiseParam2Vector_;
  std::vector<double> ThresholdVector_;
  std::vector<double> TriggerDiscriminationCenterVector_;
  std::vector<double> TriggerDiscriminationSigmaVector_;

  double TimeResolutionFast_;
  double TimeResolutionSlow_;

  bool pedestalEnabled_;

  std::vector<DetectorHit_sptr> RawHits_;
  std::list<DetectorHit_sptr> SimulatedHits_;

private:
  VDeviceSimulation(const VDeviceSimulation&) = delete;
  VDeviceSimulation(VDeviceSimulation&&) = delete;
  VDeviceSimulation& operator=(const VDeviceSimulation&) = delete;
  VDeviceSimulation& operator=(VDeviceSimulation&&) = delete;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VDeviceSimulation_H */
