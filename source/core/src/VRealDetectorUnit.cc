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
#include "G4SystemOfUnits.hh"
#include "FlagDefinition.hh"
#include "MultiChannelData.hh"
#include "VChannelMap.hh"
#include "DetectorHit.hh"

namespace {

typedef std::list<comptonsoft::DetectorHit_sptr> HitList;

bool is_group_adjacent(const HitList& group0, const HitList& group1) {
  for (const auto& hit0: group0) {
    for (const auto& hit1: group1) {
      if (hit0->isAdjacent(*hit1, true)) {
        return true;
      }
    }
  }
  return false;
}

} /* anonymous namespace */

namespace comptonsoft {

VRealDetectorUnit::VRealDetectorUnit()
  : name_(""), ID_(0), instrumentID_(0),
    sizeX_(0.0), sizeY_(0.0), sizeZ_(0.0),
    offsetX_(0.0), offsetY_(0.0), pixelPitchX_(0.0), pixelPitchY_(0.0),
    numPixelX_(1), numPixelY_(1),
    centerPosition_(0.0, 0.0, 0.0),
    directionX_(1.0, 0.0, 0.0),
    directionY_(0.0, 1.0, 0.0),
    bottomSideElectrode_(ElectrodeSide::Undefined),
    reconstructionMode_(1), clusteringOn_(true),
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

void VRealDetectorUnit::registerMultiChannelData(MultiChannelData* data)
{
  MCDVector_.emplace_back(data);
}

vector3_t VRealDetectorUnit::Position(int pixelX, int pixelY) const
{
  const vector3_t pos = centerPosition_
    + (-0.5*sizeX_ + (offsetX_+(0.5+pixelX)*pixelPitchX_)) * directionX_
    + (-0.5*sizeY_ + (offsetY_+(0.5+pixelY)*pixelPitchY_)) * directionY_;
  return pos;
}

vector3_t VRealDetectorUnit::LocalPosition(int pixelX, int pixelY) const
{
  const vector3_t dirx(1.0, 0.0, 0.0);
  const vector3_t diry(0.0, 1.0, 0.0);
  const vector3_t pos =
    (-0.5*sizeX_ + (offsetX_+(0.5+pixelX)*pixelPitchX_)) * dirx
    + (-0.5*sizeY_ + (offsetY_+(0.5+pixelY)*pixelPitchY_)) * diry;
  return pos;
}

PixelID
VRealDetectorUnit::findPixel(double localx, double localy) const
{
  if (pixelPitchX_ <= 0.0 && pixelPitchY_ <= 0.0) {
    return PixelID(PixelID::Undefined, PixelID::Undefined);
  }
  
  const double detOriginX = -0.5 * sizeX_ + offsetX_;
  const double detOriginY = -0.5 * sizeY_ + offsetY_;
  const double xReal = localx - detOriginX;
  const double yReal = localy - detOriginY;
  
  int x = std::floor(xReal/pixelPitchX_);
  int y = std::floor(yReal/pixelPitchY_);
  
  if (x<0 || y<0 || x>=numPixelX_ || y>=numPixelY_) {
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
        hit->setPHA( mcd->getPHA(channel) );
        hit->setEPI( mcd->getEPI(channel) );
        hit->setDetectorChannelID(this->getID(), section, channel);
        hit->setPixel(pixel);
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

  bool merged = true;
  while (merged) {
    merged = false;
    auto groupIter = std::begin(groups);
    while (groupIter != std::end(groups)) {
      auto groupIter1 = groupIter + 1;
      while (groupIter1 != std::end(groups)) {
        if (is_group_adjacent(*groupIter, *groupIter1)) {
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
      clusteredHits.back()->mergeAdjacentSignal(**hitIter, DetectorHit::MergedPosition::KeepLeft);
    }
  }
  hits = std::move(clusteredHits);
}

void VRealDetectorUnit::
printDetectorParameters(std::ostream& os) const
{
  os << "Geometry\n"
     << "  x: " << getSizeX()/cm << " cm\n"
     << "  y: " << getSizeY()/cm << " cm\n"
     << "  z: " << getSizeZ()/cm << " cm\n"
     << "Offset\n"
     << "  x: " << getOffsetX()/cm << " cm\n"
     << "  y: " << getOffsetY()/cm << " cm\n"
     << "Pixel\n"
     << "  number: (" << getNumPixelX() << ", " << getNumPixelY() << ")\n"
     << "  size: (" << getPixelPitchX()/cm << " cm, " << getPixelPitchY()/cm << " cm)\n"
     << "Position\n"
     << "  x: " << getCenterPositionX()/cm << " cm\n"
     << "  y: " << getCenterPositionY()/cm << " cm\n"
     << "  z: " << getCenterPositionZ()/cm << " cm\n"
     << "X-axis direction\n"
     << "  x: " << getXAxisDirectionX() << "\n"
     << "  y: " << getXAxisDirectionY() << "\n"
     << "  z: " << getXAxisDirectionZ() << "\n"
     << "Y-axis direction\n"
     << "  x: " << getYAxisDirectionX() << "\n"
     << "  y: " << getYAxisDirectionY() << "\n"
     << "  z: " << getYAxisDirectionZ() << "\n";
  os << "Number of MCD : " << NumberOfMultiChannelData() << '\n';
  os << "Reconstruction\n"
     << "  Mode : " << ReconstructionMode() << '\n'
     << "  Clustering : " << isClusteringOn() << '\n';
}

} /* namespace comptonsoft */
