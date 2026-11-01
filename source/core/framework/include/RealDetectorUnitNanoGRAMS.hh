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

#ifndef COMPTONSOFT_RealDetectorUnitNanoGRAMS_hh
#define COMPTONSOFT_RealDetectorUnitNanoGRAMS_hh 1
#include "NanoGRAMSMultiChannelData.hh"
#include "RealDetectorUnitLArTPCPixel.hh"
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>
namespace comptonsoft {

class ChargeToEnergySpline;
class NanoGRAMSTemperatureCorrection;
namespace nanograms_event_flag {
constexpr uint64_t LightGamma = 0x0001u;   // light signal is gamma-like
constexpr uint64_t LightCosmic = 0x0002u;  // light signal is cosmic-like
constexpr uint64_t LightPileup = 0x0004u;  // light pileup is found
constexpr uint64_t ExcludedCore = 0x0008u; // the highest pixel of an FEC is one of the excluded pixels
} // namespace nanograms_event_flag

class RealDetectorUnitNanoGRAMS : public RealDetectorUnitLArTPCPixel
{
public:
  // fixed readout configuration: section = FEC (VATA), channel = VATA channel
  static constexpr int NumFECs = NanoGRAMSMultiChannelData::NUM_FECS;
  static constexpr int NumChannelsPerFEC = NanoGRAMSMultiChannelData::NUM_CHANNELS;
  // light data: index = DPP channel
  static constexpr int NumLightChannels = 8;
  // TI counter of each FEC: 32 bit, 1 tick = 160 ns (wraps every ~687 s)
  static constexpr int NumTIBits = 32;
  static constexpr double TITickPeriod = 160.0 * CLHEP::ns;

public:
  RealDetectorUnitNanoGRAMS();
  virtual ~RealDetectorUnitNanoGRAMS();

  DetectorType Type() const override { return DetectorType::NanoGRAMS; }

  void initializeEvent() override;

  void setWion(double v) override;

  /**
   * recombination correction.
   * mode 4 (default of NanoGRAMS): converts the collected charge (EPI / W_ion) into the deposited energy
   * with the charge-to-energy spline of the recombination model (charge only, depending on the electric field).
   * The other modes are the same as RealDetectorUnitLArTPCPixel.
   * In any mode, Energy (and EnergyError) is set to the corrected EPI (and EPIError).
   */
  void applyRecombinationCorrection(DetectorHitVector& hits) const override;

  void setElectricField(double v);
  double ElectricField() const { return electricField_; }

  /**
   * converts the raw TI into the absolute TI/time, then selects hits.
   * TI and Time of each hit are those of its FEC.
   */
  void selectHits() override;

  // per-FEC values of the current event (index = section = FEC)
  void setRawTI(int fec, uint32_t v) { rawTI_.at(fec) = v; }
  int64_t RawTI(int fec) const { return rawTI_.at(fec); }
  // wrap-corrected TI (valid after selectHits())
  int64_t TI(int fec) const { return ti_.at(fec); }
  // absolute time = unixtime of the first event + (TI - TI of the first event) x 160 ns (valid after selectHits())
  double Time(int fec) const { return time_.at(fec); }
  void setDriftTime(int fec, double v) { driftTime_.at(fec) = v; }
  /// drift time of the FEC; NaN if the FEC number is out of range (e.g. undefined section)
  double DriftTime(int fec) const
  {
    if (fec < 0 || fec >= static_cast<int>(driftTime_.size())) {
      return std::numeric_limits<double>::quiet_NaN();
    }
    return driftTime_[fec];
  }
  // drift time counter as recorded by the DAQ (clock counts), -1 if not given
  void setRawDriftTime(int fec, uint32_t v) { rawDriftTime_.at(fec) = v; }
  int64_t RawDriftTime(int fec) const { return rawDriftTime_.at(fec); }

  // unixtime attached to the current event by the DAQ; also given to the MCDs (temperature correction)
  void setUnixTime(uint32_t v);
  uint32_t UnixTime() const { return unixTime_; }

  void determinePosition(DetectorHitVector& hits) const override;

  /**
   * temperature (gain drift) correction of the FECs, shared with the MCDs.
   * It is applied to PHA in NanoGRAMSMultiChannelData::correctPHA().
   */
  void setTemperatureCorrection(std::shared_ptr<const NanoGRAMSTemperatureCorrection> correction);
  const NanoGRAMSTemperatureCorrection* getTemperatureCorrection() const { return temperatureCorrection_.get(); }

  void setEventFlags(uint64_t v) { eventFlags_ = v; }
  uint64_t EventFlags() const { return eventFlags_; }
  void addEventFlags(uint64_t f) { eventFlags_ |= f; }
  void clearEventFlags(uint64_t f) { eventFlags_ &= ~f; }
  bool isEventFlags(uint64_t f) const { return (eventFlags_ & f) == f; }

  // light signal charge integrated in the ROI of each DPP channel (index = DPP channel), set by the light analysis
  void setLightIntegratedCharge(int dppChannel, double v) { lightIntegratedCharge_.at(dppChannel) = v; }
  double LightIntegratedCharge(int dppChannel) const { return lightIntegratedCharge_.at(dppChannel); }
  const std::vector<double>& LightIntegratedCharges() const { return lightIntegratedCharge_; }

  void setLightPeakValue(int dppChannel, double v) { lightPeakValue_.at(dppChannel) = v; }
  double LightPeakValue(int dppChannel) const { return lightPeakValue_.at(dppChannel); }
  const std::vector<double>& LightPeakValue() const { return lightPeakValue_; }

  // light signal size of the event (raw, pre-calibration; charge summed over the general analysis channels,
  // impedance-corrected and converted to an electron count), shared by all the hits of this detector
  // (set to the hits in selectHits())
  void setLightPHA(double v) { lightPHA_ = v; }
  double LightPHA() const { return lightPHA_; }

  // photon count of the event, calibrated from LightPHA with the SiPM gain
  void setPhotonCount(double v) { photonCount_ = v; }
  double PhotonCount() const { return photonCount_; }

  void setMaxDriftTime(double v) { maxDriftTime_ = v; }
  double MaxDriftTime() const { return maxDriftTime_; }

  const std::vector<std::vector<int>>& ClusterCorrespondence() const { return clusterCorrespondence_; }

  /**
   * parameters of the cluster selection. The energies are compared with EPIForSelection.
   * - a cluster core must be above ClusteringEnergyThreshold and not an excluded pixel;
   *   pixels above ClusteringSplitThreshold within ClusteringRange from the core are merged
   *   (the range is measured from the core only; merged pixels do not extend the cluster further).
   * - pixels in different FECs are merged only if the difference of the drift times is within the tolerance
   *   (negative tolerance: no merge across FECs).
   * - flags (flag::NanoGRAMS*) are set on each reconstructed hit, but no hit is removed here.
   */
  void setCrossFECMergeDriftTimeTolerance(double v) { crossFECMergeDriftTimeTolerance_ = v; }
  double CrossFECMergeDriftTimeTolerance() const { return crossFECMergeDriftTimeTolerance_; }
  void setDriftTimeLimit(double v) { driftTimeLimit_ = v; }
  double DriftTimeLimit() const { return driftTimeLimit_; }
  void setClusterPixelCountRange(int min, int max)
  {
    minClusterPixelCount_ = min;
    maxClusterPixelCount_ = max;
  }
  int MinClusterPixelCount() const { return minClusterPixelCount_; }
  int MaxClusterPixelCount() const { return maxClusterPixelCount_; }
  void setExcludedCorePixels(int fec, const std::vector<int>& channels);
  bool isExcludedCorePixel(int fec, int channel) const;

  void setRunID(int32_t runid) { run_id_ = runid; }
  int32_t runID() const { return run_id_; }

  void resetTimeTracking();

  NanoGRAMSMultiChannelData* getNanoGRAMSMultiChannelData(int i)
  {
    // Always this class has NanoGRAMSMultiChannelData
    return static_cast<NanoGRAMSMultiChannelData*>(getMultiChannelData(i));
  }

  const NanoGRAMSMultiChannelData* getNanoGRAMSMultiChannelData(int i) const
  {
    // Always this class has NanoGRAMSMultiChannelData
    return static_cast<const NanoGRAMSMultiChannelData*>(getMultiChannelData(i));
  }

protected:
  // mode 5: NanoGRAMS-specific clustering; the other modes are the same as RealDetectorUnitLArTPCPixel
  bool setReconstructionDetails(int mode) override;
  void reconstruct(const DetectorHitVector& hitSignals, DetectorHitVector& hitsReconstructed) override;

private:
  void updateTime();
  double depthFromDriftTime(double driftTime) const;
  void judgeExcludedCore();
  bool canMergeAcrossFECs(int fec1, int fec2) const;
  void clusterForNanoGRAMS(DetectorHitVector& hits, std::vector<std::vector<int>>& groups) const;
  void setClusterFlags(const DetectorHitVector& pixelHits, DetectorHitVector& clusters,
                       const std::vector<std::vector<int>>& groups) const;

private:
  std::vector<int64_t> rawTI_;
  std::vector<int64_t> ti_;
  std::vector<double> time_;
  std::vector<double> driftTime_;
  std::vector<int64_t> rawDriftTime_;
  uint32_t unixTime_;
  uint64_t eventFlags_;
  int32_t run_id_;
  std::vector<double> lightIntegratedCharge_;
  std::vector<double> lightPeakValue_;
  double lightPHA_;
  double photonCount_;
  double maxDriftTime_;
  double electricField_;
  std::unique_ptr<ChargeToEnergySpline> chargeToEnergySpline_;
  std::shared_ptr<const NanoGRAMSTemperatureCorrection> temperatureCorrection_;
  std::vector<std::vector<int>> clusterCorrespondence_;

  // cluster selection parameters
  double crossFECMergeDriftTimeTolerance_;
  double driftTimeLimit_;
  int minClusterPixelCount_;
  int maxClusterPixelCount_;
  std::vector<std::vector<int8_t>> excludedCorePixels_;

  // TI tracking over events (not reset by initializeEvent())
  std::vector<int64_t> previousTI_;
  uint32_t previousUnixTime_;
  bool timeAnchored_;
  uint32_t unixTime0_;
  std::vector<int64_t> TI0_;
};

} // namespace comptonsoft
#endif // COMPTONSOFT_RealDetectorUnitNanoGRAMS_hh
