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

#ifndef COMPTONSOFT_DetectorHit_H
#define COMPTONSOFT_DetectorHit_H 1

#include <cstdint>
#include <memory>

// If you use boost::pool library for fast memory allocation,
// set DetectorHit_BoostPool as 1; otherwise 0.
#define DetectorHit_BoostPool 1

#if DetectorHit_BoostPool
#include <boost/pool/pool.hpp>
#endif

#include "CSTypes.hh"
#include "ChannelID.hh"
#include "VoxelID.hh"
#include "PixelID.hh"
#include "FlagDefinition.hh"

namespace comptonsoft {

/**
 * A class of a hit in a semiconductor or scintillation detector.
 * This class contains an energy deposited, position, time, detector ID, and so on.
 *
 * @author Hirokazu Odaka
 * @date 2008-08-22
 * @date 2012-06-29 | rename DetectorID; add Instrument ID
 * @date 2014-09-18 | redesign.
 * @date 2016-08-19 | add trigger information, change channel ID classes
 * @date 2020-09-02 | add errors of energy/position/time
 * @date 2020-11-24 | add particle type
 * @date 2020-12-25 | add track ID
 * @date 2022-04-25 | introduce a voxel
 */
class DetectorHit
{
public:
  enum class MergedPosition { KeepLeft, Midpoint, EnergyWeighted, CopyRight };

public:
  DetectorHit() = default;
  ~DetectorHit();

  DetectorHit(const DetectorHit&) = default;
  DetectorHit(DetectorHit&&) = default;
  DetectorHit& operator=(const DetectorHit&) = default;
  DetectorHit& operator=(DetectorHit&&) = default;

  DetectorHit& operator+=(const DetectorHit& r) { return merge(r); }

  std::shared_ptr<DetectorHit> clone() const
  { return std::shared_ptr<DetectorHit>(new DetectorHit(*this)); }
  
  void setEventID(int64_t v) { eventID_ = v; }
  int64_t EventID() const { return eventID_; }

  void setTrackID(int v) { trackID_ = v; }
  int TrackID() const { return trackID_; }

  void setTimeIndicator(int64_t v) { ti_ = v; }
  int64_t TimeIndicator() const { return ti_; }
  void setTI(int64_t v) { ti_ = v; }
  int64_t TI() const { return ti_; }

  void setInstrumentID(int v) { instrumentID_ = v; }
  int InstrumentID() const { return instrumentID_; }

  void setDetectorChannelID(const DetectorBasedChannelID& v)
  { detectorChannelID_ = v; }
  void setDetectorChannelID(int detectorID, int section, int index)
  { detectorChannelID_.set(detectorID, section, index); }
  void setDetectorID(int detectorID)
  { detectorChannelID_.set(detectorID, ChannelID::Undefined, ChannelID::Undefined); }
  DetectorBasedChannelID DetectorChannelID() const { return detectorChannelID_; }
  int DetectorID() const { return detectorChannelID_.Detector(); }
  int DetectorSection() const { return detectorChannelID_.Section(); }
  int DetectorChannel() const { return detectorChannelID_.Channel(); }

  void setReadoutChannelID(const ReadoutBasedChannelID& v)
  { readoutChannelID_ = v; }
  void setReadoutChannelID(int readoutModuleID, int section, int index)
  { readoutChannelID_.set(readoutModuleID, section, index); }
  ReadoutBasedChannelID ReadoutChannelID() const { return readoutChannelID_; }
  int ReadoutModuleID() const { return readoutChannelID_.ReadoutModule(); }
  int ReadoutSection() const { return readoutChannelID_.Section(); }
  int ReadoutChannel() const { return readoutChannelID_.Channel(); }

  void setVoxel(const VoxelID& v) { voxel_ = v; }
  void setVoxel(int x, int y, int z) { voxel_.set(x, y, z); }
  int VoxelX() const { return voxel_.X(); }
  int VoxelY() const { return voxel_.Y(); }
  int VoxelZ() const { return voxel_.Z(); }
  VoxelID Voxel() const { return voxel_; }

  void setPixel(int x, int y) { voxel_.set(x, y); }
  void setPixel(const PixelID& v) { voxel_ = v; }
  int PixelX() const { return voxel_.X(); }
  int PixelY() const { return voxel_.Y(); }
  PixelID Pixel() const { return voxel_; }

  void setRawPHA(int32_t v) { rawPHA_ = v; }
  int32_t RawPHA() const { return rawPHA_; }
  void setPHA(double v) { PHA_ = v; }
  double PHA() const { return PHA_; }
  void setEPI(double v) { EPI_ = v; }
  double EPI() const { return EPI_; }
  void setEPIError(double v) { EPIError_ = v; }
  double EPIError() const { return EPIError_; }

  void setFlagData(uint64_t v) { flagData_ = v; }
  uint64_t FlagData() const { return flagData_; }
  void addFlagData(uint64_t f) { flagData_ |= f; }
  void clearFlagData(uint64_t f) { flagData_ &= ~f; }
  bool isFlagData(uint64_t f) const { return (flagData_&f)==f; }

  void setFlags(uint64_t v) { flags_ = v; }
  uint64_t Flags() const { return flags_; }
  void addFlags(uint64_t f) { flags_ |= f; }
  void clearFlags(uint64_t f) { flags_ &= ~f; }
  bool isFlags(uint64_t f) const { return (flags_&f)==f; }

  void setParticle(int v) { particle_ = v; }
  int Particle() const { return particle_; }

  void setRealTime(double v) { realTime_ = v; }
  double RealTime() const { return realTime_; }
  void setTimeGroup(int32_t v) { timeGroup_ = v; }
  int TimeGroup() const { return timeGroup_; }

  void setRealPosition(double x, double y, double z)
  { realPosition_.set(x, y, z); }
  void setRealPosition(const vector3_t& v) { realPosition_ = v; }
  double RealPositionX() const { return realPosition_.x(); }
  double RealPositionY() const { return realPosition_.y(); }
  double RealPositionZ() const { return realPosition_.z(); }
  vector3_t RealPosition() const { return realPosition_; }

  void setEnergyDeposit(double v) { energyDeposit_ = v; }
  double EnergyDeposit() const { return energyDeposit_; }
  void setEnergyCharge(double v) { energyCharge_ = v; }
  double EnergyCharge() const { return energyCharge_; }

  void setProcess(uint32_t v) { process_ = v; }
  uint32_t Process() const { return process_; }
  void addProcess(uint32_t f) { process_ |= f; }
  void clearProcess(uint32_t f) { process_ &= ~f; }
  bool isProcess(uint32_t f) const { return (process_&f)==f; }

  void setSelfTriggered(bool v);
  bool SelfTriggered() const { return isFlags(flag::SelfTriggered); }

  void setTriggered(bool v);
  bool Triggered() const { return isFlags(flag::Triggered); }

  void setSelfTriggeredTime(double v) { selfTriggeredTime_ = v; }
  double SelfTriggeredTime() const { return selfTriggeredTime_; }

  void setTriggeredTime(double v) { triggeredTime_ = v; }
  double TriggeredTime() const { return triggeredTime_; }

  void setEnergy(double v) { energy_ = v; }
  double Energy() const { return energy_; }
  void setEnergyError(double v) { energyError_ = v; }
  double EnergyError() const { return energyError_; }

  void setPosition(double x, double y, double z) { position_.set(x, y, z); }
  void setPosition(const vector3_t& v) { position_ = v; }
  double PositionX() const { return position_.x(); }
  double PositionY() const { return position_.y(); }
  double PositionZ() const { return position_.z(); }
  vector3_t Position() const { return position_; }

  void setPositionError(double x, double y, double z) { positionError_.set(x, y, z); }
  void setPositionError(const vector3_t& v) { positionError_ = v; }
  double PositionErrorX() const { return positionError_.x(); }
  double PositionErrorY() const { return positionError_.y(); }
  double PositionErrorZ() const { return positionError_.z(); }
  vector3_t PositionError() const { return positionError_; }

  void setLocalPosition(double x, double y, double z) { localPosition_.set(x, y, z); }
  void setLocalPosition(const vector3_t& v) { localPosition_ = v; }
  double LocalPositionX() const { return localPosition_.x(); }
  double LocalPositionY() const { return localPosition_.y(); }
  double LocalPositionZ() const { return localPosition_.z(); }
  vector3_t LocalPosition() const { return localPosition_; }

  void setLocalPositionError(double x, double y, double z) { localPositionError_.set(x, y, z); }
  void setLocalPositionError(const vector3_t& v) { localPositionError_ = v; }
  double LocalPositionErrorX() const { return localPositionError_.x(); }
  double LocalPositionErrorY() const { return localPositionError_.y(); }
  double LocalPositionErrorZ() const { return localPositionError_.z(); }
  vector3_t LocalPositionError() const { return localPositionError_; }

  void setTime(double v) { time_ = v; }
  double Time() const { return time_; }
  void setTimeError(double v) { time_error_ = v; }
  double TimeError() const { return time_error_; }

  void setGrade(int v) { grade_ = v; }
  int Grade() const { return grade_; }

  void setDepthSensingMode(int v) { depthSensingMode_ = v; }
  int DepthSensingMode() const { return depthSensingMode_; }
  
  /**
   * check if the given hit occurred in the same detector.
   */
  bool isInSameDetector(const DetectorHit& r) const;

  /**
   * check if the given hit occurred in the same voxel, pixel or strip.
   */
  bool isInSameVoxel(const DetectorHit& r) const;
  bool isInSamePixel(const DetectorHit& r) const { return isInSameVoxel(r); }

  /**
   * check if the given hit occurred in adjacent pixels (or strips).
   * @param r a hit to be checked.
   * @param contact if true then the adjacent pixels should share the sides.
   * otherwise, the pixels can share just an apex.
   * @return result
   */
  bool isAdjacent(const DetectorHit& r, bool contact=true) const;
  
  double distance(const DetectorHit& r) const
  { return distance(r.Position()); }
  double distance(const vector3_t& v) const
  { return (Position()-v).mag(); }
  double distance2(const DetectorHit& r) const
  { return distance2(r.Position()); }
  double distance2(const vector3_t& v) const
  { return (Position()-v).mag2(); }

  bool isXStrip() const
  { return Voxel().isXStrip(); }
  bool isYStrip() const
  { return Voxel().isYStrip(); }
  bool isPixel() const
  { return Voxel().isPixel(); }
  bool isVoxel() const
  { return Voxel().isVoxel(); }

  /**
   * merge the given hit to this object. This method is used for merging
   * two physical hits in a pixel in Monte Carlo simulations.
   */
  DetectorHit& merge(const DetectorHit& r);

  /**
   * merge the given hit signal occurring in an adjacent pixel.
   */
  DetectorHit& mergeAdjacentSignal(const DetectorHit& r,
                                   MergedPosition mergedPosition,
                                   bool setClusteredFlag=true);

  // override new/delete operators
  // by using boost::pool library for fast memory allocation
#if DetectorHit_BoostPool
  void* operator new(size_t);
  void operator delete(void*);
#endif

private:
  // Event/track ID
  int64_t eventID_ = 0l;
  int trackID_ = 0;
  // measured data
  int64_t ti_ = 0l;
  int instrumentID_ = 0;
  DetectorBasedChannelID detectorChannelID_;
  ReadoutBasedChannelID readoutChannelID_;
  VoxelID voxel_;
  int32_t rawPHA_ = 0;
  double PHA_ = 0.0;
  double EPI_ = 0.0;
  double EPIError_ = 0.0;
  uint64_t flagData_ = 0ul;
  uint64_t flags_ = 0ul;
  // simulation
  int particle_ = 0;
  double realTime_ = 0.0;
  int timeGroup_ = 0;
  vector3_t realPosition_{0.0, 0.0, 0.0};
  double energyDeposit_ = 0.0;
  double energyCharge_ = 0.0;
  uint32_t process_ = 0u;
  // trigger information
  double selfTriggeredTime_ = 0.0;
  double triggeredTime_ = 0.0;
  // reconstructed
  double energy_ = 0.0;
  double energyError_ = 0.0;
  vector3_t position_{0.0, 0.0, 0.0};
  vector3_t positionError_{0.0, 0.0, 0.0};
  vector3_t localPosition_{0.0, 0.0, 0.0};
  vector3_t localPositionError_{0.0, 0.0, 0.0};
  double time_ = 0.0;
  double time_error_ = 0.0;
  int grade_ = 0;
  int depthSensingMode_ = 0;
};

inline bool DetectorHit::isInSameDetector(const DetectorHit& r) const
{
  return ( TimeGroup() == r.TimeGroup()
           && InstrumentID() == r.InstrumentID()
           && DetectorID() == r.DetectorID() );
}

inline bool DetectorHit::isInSameVoxel(const DetectorHit& r) const
{
  return ( isInSameDetector(r) && Voxel() == r.Voxel() );
}

// override new/delete operators
// by using boost::pool library for fast memory allocation
#if DetectorHit_BoostPool
extern boost::pool<> DetectorHitAllocator;

inline
void* DetectorHit::operator new(size_t)
{
  void *aHit = (void*)DetectorHitAllocator.malloc();
  return aHit;
}

inline
void DetectorHit::operator delete(void *aHit)
{
  if (aHit != 0) DetectorHitAllocator.free(aHit);
}
#endif

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DetectorHit_H */
