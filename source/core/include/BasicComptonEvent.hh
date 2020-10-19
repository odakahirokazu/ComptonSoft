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

#ifndef COMPTONSOFT_BasicComptonEvent_H
#define COMPTONSOFT_BasicComptonEvent_H 1

#include <cstdint>
#include <memory>
#include "CSTypes.hh"
#include "ChannelID.hh"
#include "PixelID.hh"
#include "DetectorHit_sptr.hh"

namespace comptonsoft {

/**
 * A class of a reconstructed Compton event which contains information on one Compton scattering and one hit associated with the recoil electron.
 * @author Hirokazu Odaka
 * @date 2014-11-15
 * @date 2015-11-14
 * @date 2020-07-02 | add properties: total energy deposit, escape flag, reconstructed order, reconstruction fraction
 * @date 2020-07-08 | rename TotalEnergy() -> IncidentEnergy()
 * @date 2020-09-02 | add errors of energy/position
 */
class BasicComptonEvent
{
public:
  BasicComptonEvent();
  ~BasicComptonEvent();
  BasicComptonEvent(const BasicComptonEvent&) = default;
  BasicComptonEvent(BasicComptonEvent&&) = default;
  BasicComptonEvent& operator=(const BasicComptonEvent&) = default;
  BasicComptonEvent& operator=(BasicComptonEvent&&) = default;

  int64_t EventID() const { return eventID_; }
  void setEventID(int64_t v) { eventID_ = v; }
  
  int NumberOfHits() const { return numHits_; }
  void setNumberOfHits(int v) { numHits_ = v; }
  
  int Hit1ID() const { return hit1ID_; }
  uint32_t Hit1Process() const { return hit1Process_; }
  DetectorBasedChannelID Hit1DetectorChannelID() const { return hit1DetectorChannelID_; }
  int32_t Hit1DetectorID() const { return hit1DetectorChannelID_.Detector(); }
  ReadoutBasedChannelID Hit1ReadoutChannelID() const { return hit1ReadoutChannelID_; }
  PixelID Hit1Pixel() const { return hit1Pixel_; }
  double Hit1Time() const { return hit1Time_; }
  vector3_t Hit1Position() const { return hit1Position_; }
  double Hit1PositionX() const { return hit1Position_.x(); }
  double Hit1PositionY() const { return hit1Position_.y(); }
  double Hit1PositionZ() const { return hit1Position_.z(); }
  double Hit1Energy() const { return hit1Energy_; }
  double Hit1TimeError() const { return hit1TimeError_; }
  vector3_t Hit1PositionError() const { return hit1PositionError_; }
  double Hit1PositionErrorX() const { return hit1PositionError_.x(); }
  double Hit1PositionErrorY() const { return hit1PositionError_.y(); }
  double Hit1PositionErrorZ() const { return hit1PositionError_.z(); }
  double Hit1EnergyError() const { return hit1EnergyError_; }

  int Hit2ID() const { return hit2ID_; }
  uint32_t Hit2Process() const { return hit2Process_; }
  DetectorBasedChannelID Hit2DetectorChannelID() const { return hit2DetectorChannelID_; }
  int32_t Hit2DetectorID() const { return hit2DetectorChannelID_.Detector(); }
  ReadoutBasedChannelID Hit2ReadoutChannelID() const { return hit2ReadoutChannelID_; }
  PixelID Hit2Pixel() const { return hit2Pixel_; }
  double Hit2Time() const { return hit2Time_; }
  vector3_t Hit2Position() const { return hit2Position_; }
  double Hit2PositionX() const { return hit2Position_.x(); }
  double Hit2PositionY() const { return hit2Position_.y(); }
  double Hit2PositionZ() const { return hit2Position_.z(); }
  double Hit2Energy() const { return hit2Energy_; }
  double Hit2TimeError() const { return hit2TimeError_; }
  vector3_t Hit2PositionError() const { return hit2PositionError_; }
  double Hit2PositionErrorX() const { return hit2PositionError_.x(); }
  double Hit2PositionErrorY() const { return hit2PositionError_.y(); }
  double Hit2PositionErrorZ() const { return hit2PositionError_.z(); }
  double Hit2EnergyError() const { return hit2EnergyError_; }

  void setHit1(int hitID, const DetectorHit_sptr& hit);
  void setHit1ID(int v) { hit1ID_ = v; bCalc_ = false; }
  void setHit1Process(uint32_t v) { hit1Process_ = v; bCalc_ = false; }
  void setHit1DetectorChannelID(const DetectorBasedChannelID& v)
  { hit1DetectorChannelID_ = v; bCalc_ = false; }
  void setHit1ReadoutChannelID(const ReadoutBasedChannelID& v)
  { hit1ReadoutChannelID_ = v; bCalc_ = false; }
  void setHit1Pixel(const PixelID& v) { hit1Pixel_ = v; bCalc_ = false; }
  void setHit1Pixel(int x, int y) { hit1Pixel_.set(x, y); bCalc_ = false; }
  void setHit1Time(double v) { hit1Time_ = v; bCalc_ = false; }
  void setHit1Position(const vector3_t& v) { hit1Position_ = v; bCalc_ = false; }
  void setHit1Position(double x, double y, double z)
  { hit1Position_.set(x, y, z); bCalc_ = false; }
  void setHit1Energy(double v) { hit1Energy_ = v; bCalc_ = false; }
  void setHit1TimeError(double v) { hit1TimeError_ = v; bCalc_ = false; }
  void setHit1PositionError(const vector3_t& v) { hit1PositionError_ = v; bCalc_ = false; }
  void setHit1PositionError(double x, double y, double z)
  { hit1PositionError_.set(x, y, z); bCalc_ = false; }
  void setHit1EnergyError(double v) { hit1EnergyError_ = v; bCalc_ = false; }

  void setHit2(int hitID, const DetectorHit_sptr& hit);
  void setHit2ID(int v) { hit2ID_ = v; bCalc_ = false; }
  void setHit2Process(uint32_t v) { hit2Process_ = v; bCalc_ = false; }
  void setHit2DetectorChannelID(const DetectorBasedChannelID& v)
  { hit2DetectorChannelID_ = v; bCalc_ = false; }
  void setHit2ReadoutChannelID(const ReadoutBasedChannelID& v)
  { hit2ReadoutChannelID_ = v; bCalc_ = false; }
  void setHit2Pixel(const PixelID& v) { hit2Pixel_ = v; bCalc_ = false; }
  void setHit2Pixel(int x, int y) { hit2Pixel_.set(x, y); bCalc_ = false; }
  void setHit2Time(double v) { hit2Time_ = v; bCalc_ = false; }
  void setHit2Position(const vector3_t& v) { hit2Position_ = v; bCalc_ = false; }
  void setHit2Position(double x, double y, double z)
  { hit2Position_.set(x, y, z); bCalc_ = false; }
  void setHit2Energy(double v) { hit2Energy_ = v; bCalc_ = false; }
  void setHit2TimeError(double v) { hit2TimeError_ = v; bCalc_ = false; }
  void setHit2PositionError(const vector3_t& v) { hit2PositionError_ = v; bCalc_ = false; }
  void setHit2PositionError(double x, double y, double z)
  { hit2PositionError_.set(x, y, z); bCalc_ = false; }
  void setHit2EnergyError(double v) { hit2EnergyError_ = v; bCalc_ = false; }

  uint64_t Flags() const { return flags_; }
  void setFlags(uint64_t v) { flags_ = v; }
  void addFlags(uint64_t f) { flags_ |= f; }
  void clearFlags(uint64_t f) { flags_ &= ~f; }
  bool isFlags(uint64_t f) const { return (flags_&f)==f; }

  void setHitPattern(uint64_t v) { hitpattern_ = v; }
  uint64_t HitPattern() const { return hitpattern_; }
  void setGrade(int32_t v) { grade_ = v; }
  int32_t Grade() const { return grade_; }
  void setLikelihood(double v) { likelihood_ = v; }
  double Likelihood() const { return likelihood_; }

  vector3_t ConeVertex() const { return Hit1Position(); }
  
  double CosThetaE() const { refresh(); return cosThetaE_; }
  double ThetaE() const { refresh(); return thetaE_; }
  vector3_t ConeAxis() const { refresh(); return coneAxis_; }
  double ThetaG() const { refresh(); return thetaG_; }
  double DeltaTheta() const { refresh(); return deltaTheta_; }
  double PhiG() const { refresh(); return phiG_; }

  double IncidentEnergy()  const { return Hit1Energy()+Hit2Energy(); }
  bool TimeOrder() const { return (Hit1Time() <= Hit2Time()); }
  bool EnergyOrder() const { return (Hit1Energy() <= Hit2Energy()); }

  void swap();

  void setSourceDirection(const vector3_t& v)
  {
    sourceDirection_ = v;
    sourcePosition_.set(0.0, 0.0, 0.0);
    sourceDistant_ = true;
    bCalc_ = false;
  }

  void setSourcePosition(const vector3_t& v)
  {
    sourcePosition_ = v;
    sourceDirection_.set(0.0, 0.0, 1.0);
    sourceDistant_ = false;
    bCalc_ = false;
  }

  bool SourceDistant() const { return sourceDistant_; }
  vector3_t SourceDirection() const;
  vector3_t SourcePosition() const { return sourcePosition_; }

  double DistanceBetweenTheHits() const;

  void setEscapeFlag(bool f) { escapeFlag_ = f; }
  bool EscapeFlag() const { return escapeFlag_; }

  void setTotalEnergyDeposit(double v) { totalEnergyDeposit_ = v; }
  double TotalEnergyDeposit() const { return totalEnergyDeposit_; }

  void setReconstructedOrder(int i) { reconstructedOrder_ = i; }
  int ReconstructedOrder() const { return reconstructedOrder_; }

  void setReconstructionFraction(double v) { reconstructionFraction_ = v; }
  double ReconstructionFraction() const { return reconstructionFraction_; }

private:
  void refresh() const { if (!bCalc_) { calc(); } }
  void calc() const;

private:
  int64_t eventID_ = 0;
  int numHits_ = 0;
  
  int hit1ID_ = 0;
  uint32_t hit1Process_ = 0;
  DetectorBasedChannelID hit1DetectorChannelID_;
  ReadoutBasedChannelID hit1ReadoutChannelID_;
  PixelID hit1Pixel_{0, 0};
  double hit1Time_ = 0.0;
  vector3_t hit1Position_{0.0, 0.0, 0.0};
  double hit1Energy_ = 0.0;
  double hit1TimeError_ = 0.0;
  vector3_t hit1PositionError_{0.0, 0.0, 0.0};
  double hit1EnergyError_ = 0.0;

  int hit2ID_ = 0;
  uint32_t hit2Process_ = 0;
  DetectorBasedChannelID hit2DetectorChannelID_;
  ReadoutBasedChannelID hit2ReadoutChannelID_;
  PixelID hit2Pixel_{0, 0};
  double hit2Time_ = 0.0;
  vector3_t hit2Position_{0.0, 0.0, 0.0};
  double hit2Energy_ = 0.0;
  double hit2TimeError_ = 0.0;
  vector3_t hit2PositionError_{0.0, 0.0, 0.0};
  double hit2EnergyError_ = 0.0;

  uint64_t flags_ = 0ul;

  uint64_t hitpattern_ = 0ul;
  int32_t grade_ = 0;
  double likelihood_ = 0.0;

  mutable bool bCalc_ = false;
  mutable double cosThetaE_ = 1.0;
  mutable double thetaE_ = 0.0;
  mutable double thetaG_ = 0.0;
  mutable double deltaTheta_ = 0.0;
  mutable double phiG_ = 0.0;
  mutable vector3_t coneAxis_{0.0, 0.0, 1.0};

  bool sourceDistant_ = true;
  vector3_t sourceDirection_{0.0, 0.0, 1.0};
  vector3_t sourcePosition_{0.0, 0.0, 0.0};

  bool escapeFlag_ = false;
  double totalEnergyDeposit_ = 0.0;
  int reconstructedOrder_ = 0;
  double reconstructionFraction_ = 1.0;
};

template <typename T>
bool filter_compton(T (BasicComptonEvent::*getter)() const,
                    const BasicComptonEvent& event,
                    T min, T max)
{
  return min <= (event.*getter)() && (event.*getter)() <= max;
}

using BasicComptonEvent_sptr = std::shared_ptr<BasicComptonEvent>;
using const_BasicComptonEvent_sptr = std::shared_ptr<const BasicComptonEvent>;
using BasicComptonEventVector = std::vector<std::shared_ptr<BasicComptonEvent>>;

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_BasicComptonEvent_H */
