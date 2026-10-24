#include "RealDetectorUnitNanoGRAMS.hh"
#include "DetectorHit.hh"
#include "LightData.hh"
#include "NanoGRAMSChargeToEnergySpline.hh"
#include "NanoGRAMSMultiChannelData.hh"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <stdexcept>

namespace comptonsoft {

RealDetectorUnitNanoGRAMS::RealDetectorUnitNanoGRAMS()
  : unixTime_(0u),
    eventFlags_(0u),
    lightIntegratedCharge_(NumLightChannels, 0.0),
    lightPeakValue_(NumLightChannels, 0.0),
    lightPHA_(0.0),
    maxDriftTime_(67.0 * CLHEP::us),
    electricField_(0.0),
    crossFECMergeDriftTimeTolerance_(-1.0),
    driftTimeLimit_(std::numeric_limits<double>::infinity()),
    minClusterPixelCount_(1),
    maxClusterPixelCount_(std::numeric_limits<int>::max()),
    previousUnixTime_(0u),
    timeAnchored_(false),
    unixTime0_(0u)
{
  setReadoutElectrode(ElectrodeSide::Anode);
  setRecombinationCorrectionMode(4);
}

RealDetectorUnitNanoGRAMS::~RealDetectorUnitNanoGRAMS() = default;

void RealDetectorUnitNanoGRAMS::setWion(double v)
{
  RealDetectorUnitLArTPCPixel::setWion(v);
  for (int i = 0; i < NumberOfMultiChannelData(); ++i) {
    if (auto* mcd = dynamic_cast<NanoGRAMSMultiChannelData*>(getMultiChannelData(i))) {
      mcd->setWion(v);
    }
  }
}

void RealDetectorUnitNanoGRAMS::setUnixTime(uint32_t v)
{
  unixTime_ = v;
  for (int fec = 0; fec < NumberOfMultiChannelData(); ++fec) {
    getNanoGRAMSMultiChannelData(fec)->setUnixTime(v);
  }
}

void RealDetectorUnitNanoGRAMS::setTemperatureCorrection(
    std::shared_ptr<const NanoGRAMSTemperatureCorrection> correction)
{
  temperatureCorrection_ = correction;
  for (int fec = 0; fec < NumberOfMultiChannelData(); ++fec) {
    getNanoGRAMSMultiChannelData(fec)->setTemperatureCorrection(correction, fec);
  }
}

void RealDetectorUnitNanoGRAMS::initializeEvent()
{
  RealDetectorUnitLArTPCPixel::initializeEvent();

  const std::size_t numSections = NumberOfMultiChannelData();
  rawTI_.assign(numSections, -1);
  ti_.assign(numSections, -1);
  time_.assign(numSections, std::numeric_limits<double>::quiet_NaN());
  driftTime_.assign(numSections, std::numeric_limits<double>::quiet_NaN());
  rawDriftTime_.assign(numSections, -1);
  unixTime_ = 0u;
  eventFlags_ = 0u;
  std::fill(lightIntegratedCharge_.begin(), lightIntegratedCharge_.end(), 0.0);
  std::fill(lightPeakValue_.begin(), lightPeakValue_.end(), 0.0);
  lightPHA_ = 0.0;
  clusterCorrespondence_.clear();

  for (int i = 0; i < NumberOfLightData(); ++i) {
    getLightData(i)->resetEventData();
  }
}

void RealDetectorUnitNanoGRAMS::selectHits()
{
  updateTime();
  for (int fec = 0; fec < NumberOfMultiChannelData(); ++fec) {
    getMultiChannelData(fec)->setTime(time_[fec]);
  }

  RealDetectorUnitLArTPCPixel::selectHits();

  for (int i = 0; i < NumberOfDetectorHits(); ++i) {
    DetectorHit_sptr hit = getDetectorHit(i);
    const int fec = hit->DetectorSection();
    const auto* mcd = dynamic_cast<const NanoGRAMSMultiChannelData*>(getMultiChannelData(fec));
    if (mcd) {
      hit->setEPIForSelection(mcd->getEPIForSelection(hit->DetectorChannel()));
    }
    hit->setTI(ti_[fec]);
    hit->setLightPHA(lightPHA_);
    hit->setPhotonCount(photonCount_);
  }

  judgeExcludedCore();
}

void RealDetectorUnitNanoGRAMS::setExcludedCorePixels(int fec, const std::vector<int>& channels)
{
  if (excludedCorePixels_.empty()) {
    excludedCorePixels_.assign(NumFECs, std::vector<int8_t>(NumChannelsPerFEC, 0));
  }
  std::fill(excludedCorePixels_.at(fec).begin(), excludedCorePixels_.at(fec).end(), 0);
  for (const int channel : channels) {
    excludedCorePixels_.at(fec).at(channel) = 1;
  }
}

bool RealDetectorUnitNanoGRAMS::isExcludedCorePixel(int fec, int channel) const
{
  if (excludedCorePixels_.empty()) {
    return false;
  }
  return excludedCorePixels_.at(fec).at(channel) != 0;
}

void RealDetectorUnitNanoGRAMS::judgeExcludedCore()
{
  // the event is flagged if the highest pixel of an FEC is an excluded pixel
  for (int fec = 0; fec < NumberOfMultiChannelData(); ++fec) {
    const auto* mcd = dynamic_cast<const NanoGRAMSMultiChannelData*>(getMultiChannelData(fec));
    if (mcd == nullptr) {
      continue;
    }
    int maxChannel = -1;
    double maxEPI = -std::numeric_limits<double>::infinity();
    for (std::size_t channel = 0; channel < mcd->NumberOfChannels(); ++channel) {
      if (!(mcd->getDataValid(channel) && mcd->getChannelEnabled(channel))) {
        continue;
      }
      const double epi = mcd->getEPIForSelection(channel);
      if (std::isfinite(epi) && epi > maxEPI) {
        maxEPI = epi;
        maxChannel = channel;
      }
    }
    if (maxChannel >= 0 && maxEPI > ClusteringEnergyThreshold() && isExcludedCorePixel(fec, maxChannel)) {
      addEventFlags(nanograms_event_flag::ExcludedCore);
      return;
    }
  }
}

void RealDetectorUnitNanoGRAMS::setElectricField(double v)
{
  electricField_ = v;
  auto spline = std::make_unique<ChargeToEnergySpline>();
  spline->setElectricField(v / (CLHEP::volt / CLHEP::cm));
  chargeToEnergySpline_ = std::move(spline);
}

void RealDetectorUnitNanoGRAMS::applyRecombinationCorrection(DetectorHitVector& hits) const
{
  if (recombinationCorrectionMode() == 4) {
    if (!chargeToEnergySpline_) {
      throw std::runtime_error("RealDetectorUnitNanoGRAMS::applyRecombinationCorrection: electric field is not set.");
    }
    for (auto& hit : hits) {
      const double epi = hit->EPI();
      if (!(epi > 0.0)) {
        hit->setEPI(0.0);
        hit->setEPIError(0.0);
        continue;
      }
      // EPI = number of collected electrons x W_ion
      const double chargeCoulomb = epi / Wion() * CLHEP::e_SI;
      const double energy = chargeToEnergySpline_->evaluate(chargeCoulomb) * CLHEP::keV;
      const double factor = energy / epi;
      hit->setEPI(energy);
      hit->setEPIError(hit->EPIError() * factor);
    }
  }
  else {
    RealDetectorUnitLArTPCPixel::applyRecombinationCorrection(hits);
  }

  for (auto& hit : hits) {
    hit->setEnergy(hit->EPI());
    hit->setEnergyError(hit->EPIError());
  }
}

double RealDetectorUnitNanoGRAMS::depthFromDriftTime(double driftTime) const
{
  // the anode (drift time = 0) is at the top of the detector (local z = +SizeZ/2)
  return getSizeZ() * (0.5 - driftTime / maxDriftTime_);
}

void RealDetectorUnitNanoGRAMS::determinePosition(DetectorHitVector& hits) const
{
  for (auto& hit : hits) {
    const PixelID pixel = hit->Pixel();
    // section = FEC; the readout module ID is not defined for NanoGRAMS
    const int fec = hit->DetectorSection();
    const double depth = depthFromDriftTime(DriftTime(fec));

    // LArTPC has capablity to determine the position in 3D, so depth sensing mode is always on.
    hit->setPosition(PositionWithDepth(pixel, depth));
    hit->setLocalPosition(LocalPositionWithDepth(pixel, depth));
    hit->setPositionError(PositionError(hit->LocalPositionError()));
  }
}

bool RealDetectorUnitNanoGRAMS::setReconstructionDetails(int mode)
{
  if (mode == 5) {
    // NanoGRAMS-specific clustering
    setClusteringOn(true);
    return true;
  }
  return RealDetectorUnitLArTPCPixel::setReconstructionDetails(mode);
}

void RealDetectorUnitNanoGRAMS::reconstruct(const DetectorHitVector& hitSignals, DetectorHitVector& hitsReconstructed)
{
  std::transform(hitSignals.begin(), hitSignals.end(), std::back_inserter(hitsReconstructed),
                 [](const DetectorHit_sptr& hit) {
                   auto hit2 = hit->clone();
                   hit2->setEnergy(hit->EPI());
                   hit2->setEnergyError(hit->EPIError());
                   return hit2;
                 });
  determinePosition(hitsReconstructed);

  clusterCorrespondence_.clear();
  if (!isClusteringOn()) {
    for (std::size_t i = 0; i < hitsReconstructed.size(); ++i) {
      clusterCorrespondence_.push_back({static_cast<int>(i)});
    }
    setClusterFlags(hitSignals, hitsReconstructed, clusterCorrespondence_);
  }
  else if (ReconstructionMode() == 5) {
    clusterForNanoGRAMS(hitsReconstructed, clusterCorrespondence_);
    setClusterFlags(hitSignals, hitsReconstructed, clusterCorrespondence_);
  }
  else {
    // generic clustering: the pixel correspondence is not available, so ClusterCorrespondence() is left empty
    // and no NanoGRAMS cluster flags are set (all the clusters are regarded as gamma-ray events).
    if (ReconstructionMode() == 4) {
      clusterByThreshold(hitsReconstructed);
    }
    else {
      cluster(hitsReconstructed);
    }
  }

  // EnergyCharge keeps the charge-equivalent energy (charge x W_ion) before the recombination correction
  for (auto& hit : hitsReconstructed) {
    hit->setEnergyCharge(hit->EPI());
  }

  correctPhotonDetectionEfficiency(hitsReconstructed);
  if (recombinationCorrectionMode() > 0) {
    applyRecombinationCorrection(hitsReconstructed);
  }
}

bool RealDetectorUnitNanoGRAMS::canMergeAcrossFECs(int fec1, int fec2) const
{
  if (crossFECMergeDriftTimeTolerance_ < 0.0) {
    return false;
  }
  const double difference = std::abs(DriftTime(fec1) - DriftTime(fec2));
  return std::isfinite(difference) && difference <= crossFECMergeDriftTimeTolerance_;
}

void RealDetectorUnitNanoGRAMS::clusterForNanoGRAMS(DetectorHitVector& hits,
                                                    std::vector<std::vector<int>>& groups) const
{
  const double coreThreshold = ClusteringEnergyThreshold();
  const double splitThreshold = ClusteringSplitThreshold();
  const int range = ClusteringRange();
  const int numHits = hits.size();

  std::vector<int> candidates;
  std::vector<int> seeds;
  for (int i = 0; i < numHits; ++i) {
    const double epi = hits[i]->EPIForSelection();
    if (epi < splitThreshold) {
      continue;
    }
    candidates.push_back(i);
    // excluded pixels cannot seed a cluster, but can be absorbed by a real core
    if (epi > coreThreshold && !isExcludedCorePixel(hits[i]->DetectorSection(), hits[i]->DetectorChannel())) {
      seeds.push_back(i);
    }
  }
  std::stable_sort(seeds.begin(), seeds.end(),
                   [&hits](int a, int b) { return hits[a]->EPIForSelection() > hits[b]->EPIForSelection(); });

  std::vector<uint8_t> used(numHits, 0);
  DetectorHitVector clusters;
  groups.clear();
  for (const int seed : seeds) {
    if (used[seed]) {
      continue;
    }
    used[seed] = 1;
    std::vector<int> group{seed};
    // only the pixels within the range from the core are merged (no chaining through the merged pixels)
    const DetectorHit_sptr& core = hits[seed];
    for (const int j : candidates) {
      if (used[j]) {
        continue;
      }
      const DetectorHit_sptr& neighbor = hits[j];
      if (std::abs(neighbor->VoxelX() - core->VoxelX()) > range ||
          std::abs(neighbor->VoxelY() - core->VoxelY()) > range) {
        continue;
      }
      if (neighbor->DetectorSection() != core->DetectorSection() &&
          !canMergeAcrossFECs(core->DetectorSection(), neighbor->DetectorSection())) {
        continue;
      }
      used[j] = 1;
      group.push_back(j);
    }

    // the core (highest EPIForSelection) represents the cluster (channel ID, FEC)
    DetectorHit_sptr merged = hits[seed]->clone();
    for (std::size_t k = 1; k < group.size(); ++k) {
      merged->mergeAdjacentSignal(*hits[group[k]], DetectorHit::MergedPosition::EnergyWeighted);
    }
    clusters.push_back(merged);
    groups.push_back(std::move(group));
  }

  hits = std::move(clusters);
}

void RealDetectorUnitNanoGRAMS::setClusterFlags(const DetectorHitVector& pixelHits, DetectorHitVector& clusters,
                                                const std::vector<std::vector<int>>& groups) const
{
  const double coreThreshold = ClusteringEnergyThreshold();
  for (std::size_t c = 0; c < clusters.size(); ++c) {
    DetectorHit_sptr& cluster = clusters[c];
    const std::vector<int>& group = groups[c];
    const int fec = cluster->DetectorSection();

    const double driftTime = DriftTime(fec);
    if (!std::isfinite(driftTime) || driftTime >= driftTimeLimit_) {
      cluster->addFlags(flag::NanoGRAMSTimeUp);
    }

    const int numPixels = group.size();
    if (numPixels < minClusterPixelCount_ || numPixels > maxClusterPixelCount_) {
      cluster->addFlags(flag::NanoGRAMSPixelCountOutOfRange);
    }

    if (numPixels == 3) {
      const DetectorHit& p1 = *pixelHits[group[0]];
      const DetectorHit& p2 = *pixelHits[group[1]];
      const DetectorHit& p3 = *pixelHits[group[2]];
      if ((p2.VoxelX() - p1.VoxelX()) * (p3.VoxelY() - p1.VoxelY()) ==
          (p2.VoxelY() - p1.VoxelY()) * (p3.VoxelX() - p1.VoxelX())) {
        cluster->addFlags(flag::NanoGRAMSCollinear);
      }
    }

    for (int j = 0; j < static_cast<int>(pixelHits.size()); ++j) {
      const DetectorHit& pixel = *pixelHits[j];
      if (pixel.DetectorSection() != fec || std::find(group.begin(), group.end(), j) != group.end() ||
          isExcludedCorePixel(fec, pixel.DetectorChannel())) {
        continue;
      }
      if (pixel.EPIForSelection() > coreThreshold) {
        cluster->addFlags(flag::NanoGRAMSMultipleClustersInFEC);
        break;
      }
    }
  }
}

void RealDetectorUnitNanoGRAMS::resetTimeTracking()
{
  previousTI_.clear();
  previousUnixTime_ = 0u;
  timeAnchored_ = false;
  unixTime0_ = 0u;
  TI0_.clear();
}

void RealDetectorUnitNanoGRAMS::updateTime()
{
  const std::size_t numSections = NumberOfMultiChannelData();
  if (previousTI_.size() != numSections) {
    previousTI_.assign(numSections, -1);
  }

  constexpr int64_t PERIOD = int64_t{1} << NumTIBits;
  const bool unixTimeAvailable = (unixTime_ != 0u && previousUnixTime_ != 0u && unixTime_ >= previousUnixTime_);

  for (std::size_t fec = 0; fec < numSections; ++fec) {
    const int64_t raw = rawTI_[fec];
    if (raw < 0) {
      continue;
    }

    const int64_t previous = previousTI_[fec];
    if (previous < 0) {
      ti_[fec] = raw;
    }
    else {
      const int64_t previousRaw = previous % PERIOD;
      int64_t numWraps = (raw < previousRaw) ? 1 : 0;
      if (unixTimeAvailable) {
        // choose the number of wraps that is closest to the elapsed unixtime.
        // the unixtime has a lag of a few seconds, which is much shorter than the wrap period.
        const double elapsedTicks = (unixTime_ - previousUnixTime_) * CLHEP::s / TITickPeriod;
        numWraps = std::max<int64_t>(
            0, std::llround((elapsedTicks - static_cast<double>(raw - previousRaw)) / static_cast<double>(PERIOD)));
      }
      ti_[fec] = (previous - previousRaw) + numWraps * PERIOD + raw;
    }
    previousTI_[fec] = ti_[fec];
  }

  if (unixTime_ != 0u) {
    previousUnixTime_ = unixTime_;
  }

  if (!timeAnchored_) {
    unixTime0_ = unixTime_;
    TI0_ = ti_;
    timeAnchored_ = true;
  }

  for (std::size_t fec = 0; fec < numSections; ++fec) {
    if (ti_[fec] < 0 || TI0_[fec] < 0) {
      time_[fec] = std::numeric_limits<double>::quiet_NaN();
      continue;
    }
    time_[fec] = unixTime0_ * CLHEP::s + (ti_[fec] - TI0_[fec]) * TITickPeriod;
  }
}

} // namespace comptonsoft
