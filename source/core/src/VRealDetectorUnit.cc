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

#include "VRealDetectorUnit.hh"

#include <list>
#include <algorithm>
#include <iterator>
#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "MultiChannelData.hh"
#include "VChannelMap.hh"
#include "DetectorHit.hh"
#include "FrameData.hh"

namespace unit = anlgeant4::unit;

namespace {

typedef std::list<comptonsoft::DetectorHit_sptr> HitList;

bool is_group_adjacent(const HitList& group0, const HitList& group1, bool contact_condition) {
  for (const auto& hit0: group0) {
    for (const auto& hit1: group1) {
      if (hit0->isAdjacent(*hit1, contact_condition)) {
        return true;
      }
    }
  }
  return false;
}

comptonsoft::DetectorHit_sptr merge_hits(const std::vector<comptonsoft::DetectorHit_sptr>& group) {
  comptonsoft::DetectorHit_sptr merged_hit = group.front()->clone();
  double sumE = merged_hit->EPI();
  double maxEpi = merged_hit->EPI();
  comptonsoft::vector3_t pos = merged_hit->Position() * sumE;
  comptonsoft::vector3_t localPos = merged_hit->LocalPosition() * sumE;
  comptonsoft::vector3_t realPos = merged_hit->RealPosition() * sumE;
  for (const auto &hit: group) {
    if (hit == group.front()) {
      continue;
    }
    const double epi = hit->EPI();
    sumE += epi;
    localPos += hit->LocalPosition() * epi;
    pos += hit->Position() * epi;
    realPos += hit->RealPosition() * epi;
    if (epi > maxEpi) {
      maxEpi = epi;
      merged_hit->setPositionError(hit->PositionError());
      merged_hit->setLocalPositionError(hit->LocalPositionError());
      merged_hit->setVoxel(hit->Voxel());
    }
    merged_hit->mergeAdjacentSignal(*hit, comptonsoft::DetectorHit::MergedPosition::EnergyWeighted);
  }
  merged_hit->setLocalPosition(localPos / sumE);
  merged_hit->setPosition(pos / sumE);
  merged_hit->setRealPosition(realPos / sumE);
  merged_hit->setEPI(sumE);
  return merged_hit;
}
} /* anonymous namespace */

namespace comptonsoft {

VRealDetectorUnit::VRealDetectorUnit()
    : name_(""), ID_(0), instrumentID_(0),
      sizeX_(0.0), sizeY_(0.0), sizeZ_(0.0),
      offsetX_(0.0), offsetY_(0.0), offsetZ_(0.0),
      voxelPitchX_(0.0), voxelPitchY_(0.0), voxelPitchZ_(0.0),
      numVoxelX_(1), numVoxelY_(1), numVoxelZ_(1),
      centerPosition_(0.0, 0.0, 0.0),
      directionX_(1.0, 0.0, 0.0),
      directionY_(0.0, 1.0, 0.0),
      directionZ_(0.0, 0.0, 1.0),
      bottomSideElectrode_(ElectrodeSide::Undefined),
      reconstructionMode_(1), clusteringOn_(true), clusteringContactCondition_(true), clusteringEnergyThreshold_(0.0), clusteringSplitThreshold_(0.0), clusteringRange_(2),
    channelMap_(nullptr)
{
}

VRealDetectorUnit::~VRealDetectorUnit() = default;

std::string VRealDetectorUnit::getNamePrefix() const
{
  std::string name = getName();
  size_t c = name.find(':');
  if (c == std::string::npos) {
    return name;
  }
  return std::string(name, 0, c);
}

void VRealDetectorUnit::registerMultiChannelData(std::unique_ptr<MultiChannelData>&& mcd)
{
  MCDVector_.push_back(std::move(mcd));
}

vector3_t VRealDetectorUnit::Position(int voxelX, int voxelY, int voxelZ) const
{
  const vector3_t pos = centerPosition_
    + (-0.5*sizeX_ + (offsetX_+(0.5+voxelX)*voxelPitchX_)) * directionX_
    + (-0.5*sizeY_ + (offsetY_+(0.5+voxelY)*voxelPitchY_)) * directionY_
    + (-0.5*sizeZ_ + (offsetZ_+(0.5+voxelZ)*voxelPitchZ_)) * directionZ_;
  return pos;
}

vector3_t VRealDetectorUnit::Position(int pixelX, int pixelY) const
{
  const vector3_t pos = centerPosition_
    + (-0.5*sizeX_ + (offsetX_+(0.5+pixelX)*voxelPitchX_)) * directionX_
    + (-0.5*sizeY_ + (offsetY_+(0.5+pixelY)*voxelPitchY_)) * directionY_;
  return pos;
}

vector3_t VRealDetectorUnit::LocalPosition(int voxelX, int voxelY, int voxelZ) const
{
  const vector3_t dirx(1.0, 0.0, 0.0);
  const vector3_t diry(0.0, 1.0, 0.0);
  const vector3_t dirz(0.0, 0.0, 1.0);
  const vector3_t pos =
    (-0.5*sizeX_ + (offsetX_+(0.5+voxelX)*voxelPitchX_)) * dirx
    + (-0.5*sizeY_ + (offsetY_+(0.5+voxelY)*voxelPitchY_)) * diry
    + (-0.5*sizeZ_ + (offsetZ_+(0.5+voxelZ)*voxelPitchZ_)) * dirz;
  return pos;
}

vector3_t VRealDetectorUnit::LocalPosition(int pixelX, int pixelY) const
{
  const vector3_t dirx(1.0, 0.0, 0.0);
  const vector3_t diry(0.0, 1.0, 0.0);
  const vector3_t pos =
    (-0.5*sizeX_ + (offsetX_+(0.5+pixelX)*voxelPitchX_)) * dirx
    + (-0.5*sizeY_ + (offsetY_+(0.5+pixelY)*voxelPitchY_)) * diry;
  return pos;
}

vector3_t VRealDetectorUnit::PositionWithDepth(int pixelX, int pixelY, double localz) const
{
  const vector3_t pos = centerPosition_
    + (-0.5*sizeX_ + (offsetX_+(0.5+pixelX)*voxelPitchX_)) * directionX_
    + (-0.5*sizeY_ + (offsetY_+(0.5+pixelY)*voxelPitchY_)) * directionY_
    + localz * directionZ_;
  return pos;
}

vector3_t VRealDetectorUnit::LocalPositionWithDepth(int pixelX, int pixelY, double localz) const
{
  const vector3_t dirx(1.0, 0.0, 0.0);
  const vector3_t diry(0.0, 1.0, 0.0);
  const vector3_t dirz(0.0, 0.0, 1.0);
  const vector3_t pos =
    (-0.5*sizeX_ + (offsetX_+(0.5+pixelX)*voxelPitchX_)) * dirx
    + (-0.5*sizeY_ + (offsetY_+(0.5+pixelY)*voxelPitchY_)) * diry
    + localz * dirz;
  return pos;
}

vector3_t VRealDetectorUnit::LocalPositionError() const
{
  const double conversionToSigma = 1.0/std::sqrt(12.0);
  const double dx = getPixelPitchX()*conversionToSigma;
  const double dy = getPixelPitchY()*conversionToSigma;
  const double dz = getThickness()*conversionToSigma;
  return vector3_t(dx, dy, dz);
}

vector3_t VRealDetectorUnit::PositionError(const vector3_t& localError) const
{
  const vector3_t v = localError.x() * directionX_
    + localError.y() * directionY_
    + localError.z() * directionZ_;
  return v;
}

VoxelID
VRealDetectorUnit::findVoxel(double localx, double localy, double localz) const
{
  if (voxelPitchX_ <= 0.0 || voxelPitchY_ <= 0.0 || voxelPitchZ_ <= 0.0) {
    return VoxelID(VoxelID::Undefined, VoxelID::Undefined, VoxelID::Undefined);
  }

  const double detOriginX = -0.5 * sizeX_ + offsetX_;
  const double detOriginY = -0.5 * sizeY_ + offsetY_;
  const double detOriginZ = -0.5 * sizeZ_ + offsetZ_;
  const double xReal = localx - detOriginX;
  const double yReal = localy - detOriginY;
  const double zReal = localz - detOriginZ;

  int x = std::floor(xReal/voxelPitchX_);
  int y = std::floor(yReal/voxelPitchY_);
  int z = std::floor(zReal/voxelPitchZ_);

  if (x<0 || y<0 || z<0 || x>=numVoxelX_ || y>=numVoxelY_ || z>=numVoxelZ_) {
    x = PixelID::OnMergin;
    y = PixelID::OnMergin;
    z = PixelID::OnMergin;
  }

  return VoxelID(x, y, z);
}

PixelID
VRealDetectorUnit::findPixel(double localx, double localy) const
{
  if (voxelPitchX_ <= 0.0 || voxelPitchY_ <= 0.0) {
    return PixelID(PixelID::Undefined, PixelID::Undefined);
  }

  const double detOriginX = -0.5 * sizeX_ + offsetX_;
  const double detOriginY = -0.5 * sizeY_ + offsetY_;
  const double xReal = localx - detOriginX;
  const double yReal = localy - detOriginY;

  int x = std::floor(xReal/voxelPitchX_);
  int y = std::floor(yReal/voxelPitchY_);

  if (x<0 || y<0 || x>=numVoxelX_ || y>=numVoxelY_) {
    x = PixelID::OnMergin;
    y = PixelID::OnMergin;
  }

  return PixelID(x, y);
}

PixelID VRealDetectorUnit::ChannelToStripPair(const int hit1Section,
                                              const int hit1Channel,
                                              const int hit2Section,
                                              const int hit2Channel) const
{
  if (hit1Section<0 || hit1Channel<0) {
    return PixelID(PixelID::Undefined, PixelID::Undefined);
  }

  PixelID pixel = channelMap_->getPixel(hit1Section, hit1Channel);

  if (hit2Section<0 || hit2Channel<0) {
    return pixel;
  }

  PixelID pixel2 = channelMap_->getPixel(hit2Section, hit2Channel);
  return pixel.combine(pixel2);
}

PixelID VRealDetectorUnit::ChannelToPixel(int hitSection, int hitChannel) const
{
  if (hitSection<0 || hitChannel<0) {
    return PixelID(PixelID::Undefined, PixelID::Undefined);
  }
  return channelMap_->getPixel(hitSection, hitChannel);
}

SectionChannelPair VRealDetectorUnit::PixelToChanelID(const PixelID& pixel) const
{
  return channelMap_->getSectionChannel(pixel.X(), pixel.Y());
}

void VRealDetectorUnit::initializeEvent()
{
  for (auto& mcd: MCDVector_) {
    mcd->resetEventData();
    mcd->resetDataValidVector(0);
  }

  detectorHits_.clear();
  reconstructedHits_.clear();
}

void VRealDetectorUnit::selectHits()
{
  const int NumMCD = NumberOfMultiChannelData();
  for (int section=0; section<NumMCD; section++) {
    MultiChannelData* mcd = getMultiChannelData(section);
    mcd->selectHits();
    const bool anode = mcd->isAnodeSide();
    const bool cathode = mcd->isCathodeSide();
    const bool prioritySide = mcd->isPrioritySide();
    const std::size_t NumChannels = mcd->NumberOfChannels();
    for (std::size_t channel=0; channel<NumChannels; channel++) {
      if (mcd->getChannelHit(channel)) {
        DetectorHit_sptr hit(new DetectorHit);
        hit->setTime(mcd->Time());
        hit->setFlagData(mcd->Flags());
        PixelID pixel = ChannelToPixel(section, channel);
        hit->setRawPHA( mcd->getRawADC(channel) );
        hit->setPHA( mcd->getPHA(channel) );
        hit->setEPI( mcd->getEPI(channel) );
        hit->setDetectorChannelID(this->getID(), section, channel);
        hit->setPixel(pixel);
        hit->setLocalPositionError(LocalPositionError());
        ReadoutBasedChannelID readout = mcd->ReadoutID();
        hit->setReadoutChannelID(readout.ReadoutModule(),
                                 readout.Section(),
                                 channel);
        if (anode) { hit->addFlags(flag::AnodeSide); }
        if (cathode) { hit->addFlags(flag::CathodeSide); }
        if (prioritySide) { hit->addFlags(flag::PrioritySide); }
        
        insertDetectorHit(hit);
      }
    }
  }
}

void VRealDetectorUnit::discriminateHits()
{
  auto it = detectorHits_.begin();
  while (it != detectorHits_.end()) {
    const int section = (*it)->DetectorSection();
    const int channel = (*it)->DetectorChannel();
    if (section==ChannelID::Undefined) {
      ++it;
      continue;
    }

    const double energy = (*it)->EPI();
    const MultiChannelData* mcd = getMultiChannelData(section);
    const bool selected = mcd->discriminate(channel, energy);
    if (selected) {
      ++it;
    }
    else {
      it = detectorHits_.erase(it);
    }
  }
}

void VRealDetectorUnit::reconstructHits()
{
  if (ReconstructionMode() == 0) {
    reconstructedHits_ = detectorHits_;
  }
  else {
    reconstruct(detectorHits_, reconstructedHits_);
  }
}

void VRealDetectorUnit::recalculateEPI()
{
  for (DetectorHit_sptr& hit: detectorHits_) {
    const int section = hit->DetectorSection();
    const int channel = hit->DetectorChannel();
    const double vPHA = hit->PHA();
    const MultiChannelData* mcd = getMultiChannelData(section);
    const double vEPI = mcd->PHA2EPI(channel, vPHA);
    hit->setEPI(vEPI);
  }
}

void VRealDetectorUnit::assignReadoutInfo()
{
  if (channelMap_==nullptr) return;
  
  for (DetectorHit_sptr& hit: detectorHits_) {
    const PixelID pixel = hit->Pixel();
    const SectionChannelPair detectorChannel = PixelToChanelID(pixel);
    const int section = detectorChannel.Section();
    const int channel = detectorChannel.Channel();
    hit->setDetectorChannelID(this->getID(), section, channel);
    const MultiChannelData* mcd = getMultiChannelData(section);
    if (mcd) {
      ReadoutBasedChannelID readout = mcd->ReadoutID();
      hit->setReadoutChannelID(readout.ReadoutModule(),
                               readout.Section(),
                               channel);
      if (mcd->isAnodeSide()) { hit->addFlags(flag::AnodeSide); }
      if (mcd->isCathodeSide()) { hit->addFlags(flag::CathodeSide); }
      if (mcd->isPrioritySide()) { hit->addFlags(flag::PrioritySide); }
    }
  }
}

void VRealDetectorUnit::cluster(DetectorHitVector& hits) const
{
  std::vector<std::list<DetectorHit_sptr>> groups;
  std::transform(hits.begin(), hits.end(), std::back_inserter(groups),
                 [](const DetectorHit_sptr& hit){
                   return std::list<DetectorHit_sptr>(1, hit);
                 });

  auto compair = [](const DetectorHit_sptr& hit1, const DetectorHit_sptr& hit2)-> bool {
    return hit1->EPI() > hit2->EPI();
  };

  const bool contact_condition = ClusteringContactCondition();

  bool merged = true;
  while (merged) {
    merged = false;
    auto groupIter = std::begin(groups);
    while (groupIter != std::end(groups)) {
      auto groupIter1 = groupIter + 1;
      while (groupIter1 != std::end(groups)) {
        if (is_group_adjacent(*groupIter, *groupIter1, contact_condition)) {
          groupIter->merge(*groupIter1, compair);
          groupIter1 = groups.erase(groupIter1);
          merged = true;
        }
        else {
          ++groupIter1;
        }
      }
      ++groupIter;
    }
  }

  for (auto& group: groups) {
    group.sort(compair);
  }

  DetectorHitVector clusteredHits;
  for (auto& group: groups) {
    clusteredHits.push_back(DetectorHit_sptr(group.front()));
    auto hitIter=std::begin(group);
    ++hitIter;
    for (; hitIter!=std::end(group); ++hitIter) {
      clusteredHits.back()->mergeAdjacentSignal(**hitIter, DetectorHit::MergedPosition::EnergyWeighted);
    }
  }
  hits = std::move(clusteredHits);
}

void VRealDetectorUnit::clusterByThreshold(DetectorHitVector &hits) const {
  const auto energyThreshold = ClusteringEnergyThreshold();
  const auto splitThreshold  = ClusteringSplitThreshold();
  const auto clusteringRange = ClusteringRange();

  // Cash the number of voxels in each dimension locally to avoid repeated function calls inside the loop
  const int NX = getNumVoxelX();
  const int NY = getNumVoxelY();
  const int NZ = getNumVoxelZ();
  const int clusteringRange_z = (NZ > 1 ? clusteringRange : 0);

  auto voxelToPix = [&](int x, int y, int z) {
    return x + y * NX + z * NX * NY;
  };

  // thread_local variable for pixel mapping to avoid repeated memory allocation and deallocation across events
  const int totalVoxels = NX * NY * NZ;
  static thread_local std::vector<int> pixelMapping;
  if ((int)pixelMapping.size() < totalVoxels) {
    pixelMapping.assign(totalVoxels, -1);
  }

  std::vector<int> usedPixels;
  std::vector<int> splitIdx;
  std::vector<int> mainIdx;
  const int num_hits = (int)hits.size();

  splitIdx.reserve(num_hits);
  mainIdx.reserve(num_hits);
  usedPixels.reserve(num_hits);

  for (int i = 0; i < num_hits; ++i) {
    if (hits[i]->EPI() < splitThreshold) continue;
    const int x = hits[i]->VoxelX();
    const int y = hits[i]->VoxelY();
    const int z = hits[i]->isVoxel() ? hits[i]->VoxelZ() : 0;
    const int idx = voxelToPix(x, y, z);
    if (pixelMapping[idx] == -1) {
      pixelMapping[idx] = (int)splitIdx.size();
      splitIdx.push_back(i);
      usedPixels.push_back(idx);
    }
  }

  const int M = (int)splitIdx.size();
  if (M == 0) {
    // Reset only the entries corresponding to used pixels
    for (int idx : usedPixels) pixelMapping[idx] = -1;
    hits.clear();
    return;
  }

  for (int ii = 0; ii < M; ++ii) {
    if (hits[splitIdx[ii]]->EPI() >= energyThreshold) {
      mainIdx.push_back(ii);
    }
  }

  std::sort(mainIdx.begin(), mainIdx.end(), [&](int a, int b) {
    return hits[splitIdx[a]]->EPI() > hits[splitIdx[b]]->EPI();
  });

  std::vector<uint8_t> visited(M, 0);
  DetectorHitVector clusteredHits;
  clusteredHits.reserve(mainIdx.size());

  
  std::vector<int> to_merge;
  to_merge.reserve(M);

  std::vector<DetectorHit_sptr> group;
  group.reserve(64);

  for (int main_index : mainIdx) {
    if (visited[main_index]) continue;

    visited[main_index] = 1;
    to_merge.clear();
    to_merge.push_back(main_index);
    group.clear();
    group.push_back(hits[splitIdx[main_index]]);

    int head = 0;
    while (head < (int)to_merge.size()) {
      const int current_index = to_merge[head++];
      const auto &current_hit = hits[splitIdx[current_index]];
      const int x = current_hit->VoxelX();
      const int y = current_hit->VoxelY();
      const int z = current_hit->VoxelZ();

      for (int dx = -clusteringRange; dx <= clusteringRange; ++dx) {
        const int nx = x + dx;
        if (nx < 0 || nx >= NX) continue;
        for (int dy = -clusteringRange; dy <= clusteringRange; ++dy) {
          const int ny = y + dy;
          if (ny < 0 || ny >= NY) continue;
          for (int dz = -clusteringRange_z; dz <= clusteringRange_z; ++dz) {
            const int nz = z + dz;
            if (nz < 0 || nz >= NZ) continue;

            const int neighbor_split_idx = pixelMapping[voxelToPix(nx, ny, nz)];
            if (neighbor_split_idx == -1 || visited[neighbor_split_idx]) continue;

            visited[neighbor_split_idx] = 1;
            to_merge.push_back(neighbor_split_idx);
            group.push_back(hits[splitIdx[neighbor_split_idx]]);
          }
        }
      }
    }
    std::sort(group.begin(), group.end(), [](const DetectorHit_sptr &a, const DetectorHit_sptr &b) {
      return a->EPI() > b->EPI();
    });
    clusteredHits.push_back(merge_hits(group));
  }

  for (int idx : usedPixels) pixelMapping[idx] = -1;

  hits = std::move(clusteredHits);
}

void VRealDetectorUnit::
printDetectorParameters(std::ostream& os) const
{
  os << "Geometry\n"
     << "  x: " << getSizeX()/unit::cm << " cm\n"
     << "  y: " << getSizeY()/unit::cm << " cm\n"
     << "  z: " << getSizeZ()/unit::cm << " cm\n"
     << "Offset\n"
     << "  x: " << getOffsetX()/unit::cm << " cm\n"
     << "  y: " << getOffsetY()/unit::cm << " cm\n"
     << "Pixel\n"
     << "  number: (" << getNumPixelX() << ", " << getNumPixelY() << ")\n"
     << "  size: (" << getPixelPitchX()/unit::cm << " cm, " << getPixelPitchY()/unit::cm << " cm)\n"
     << "Position\n"
     << "  x: " << getCenterPositionX()/unit::cm << " cm\n"
     << "  y: " << getCenterPositionY()/unit::cm << " cm\n"
     << "  z: " << getCenterPositionZ()/unit::cm << " cm\n"
     << "X-axis direction\n"
     << "  x: " << getXAxisDirectionX() << "\n"
     << "  y: " << getXAxisDirectionY() << "\n"
     << "  z: " << getXAxisDirectionZ() << "\n"
     << "Y-axis direction\n"
     << "  x: " << getYAxisDirectionX() << "\n"
     << "  y: " << getYAxisDirectionY() << "\n"
     << "  z: " << getYAxisDirectionZ() << "\n"
     << "Z-axis direction\n"
     << "  x: " << getZAxisDirectionX() << "\n"
     << "  y: " << getZAxisDirectionY() << "\n"
     << "  z: " << getZAxisDirectionZ() << "\n";
  os << "Number of MCD : " << NumberOfMultiChannelData() << '\n';
  os << "Frame data: " << hasFrameData() << '\n';
  os << "Reconstruction\n"
     << "  Mode : " << ReconstructionMode() << '\n'
     << "  Clustering : " << isClusteringOn() << '\n'
     << "  Contact condition : " << ClusteringContactCondition() << '\n'
     << "  Event Threshold : " << ClusteringEnergyThreshold() / unit::keV << " keV\n"
     << "  Split Threshold : " << ClusteringSplitThreshold() / unit::keV << " keV\n"
     << "  Clustering Range : " << ClusteringRange() << '\n';
}

void VRealDetectorUnit::registerFrameData(std::unique_ptr<FrameData> &&frame) {
  frame_ = std::move(frame);
}

} /* namespace comptonsoft */
