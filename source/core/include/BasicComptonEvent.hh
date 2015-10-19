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
  DetectorChannelID Hit1DetectorChannel() const { return hit1DetectorChannel_; }
  int32_t Hit1DetectorID() const { return hit1DetectorChannel_.Detector(); }
  ReadoutChannelID Hit1ReadoutChannel() const { return hit1ReadoutChannel_; }
  PixelID Hit1Pixel() const { return hit1Pixel_; }
  double Hit1Time() const { return hit1Time_; }
  vector3_t Hit1Position() const { return hit1Position_; }
  double Hit1PositionX() const { return hit1Position_.x(); }
  double Hit1PositionY() const { return hit1Position_.y(); }
  double Hit1PositionZ() const { return hit1Position_.z(); }
  double Hit1Energy() const { return hit1Energy_; }

  int Hit2ID() const { return hit2ID_; }
  uint32_t Hit2Process() const { return hit2Process_; }
  DetectorChannelID Hit2DetectorChannel() const { return hit2DetectorChannel_; }
  int32_t Hit2DetectorID() const { return hit2DetectorChannel_.Detector(); }
  ReadoutChannelID Hit2ReadoutChannel() const { return hit2ReadoutChannel_; }
  PixelID Hit2Pixel() const { return hit2Pixel_; }
  double Hit2Time() const { return hit2Time_; }
  vector3_t Hit2Position() const { return hit2Position_; }
  double Hit2PositionX() const { return hit2Position_.x(); }
  double Hit2PositionY() const { return hit2Position_.y(); }
  double Hit2PositionZ() const { return hit2Position_.z(); }
  double Hit2Energy() const { return hit2Energy_; }

  void setHit1(int hitID, const DetectorHit_sptr& hit);
  void setHit1ID(int v) { hit1ID_ = v; bCalc_ = false; }
  void setHit1Process(uint32_t v) { hit1Process_ = v; bCalc_ = false; }
  void setHit1DetectorChannel(const DetectorChannelID& v)
  { hit1DetectorChannel_ = v; bCalc_ = false; }
  void setHit1ReadoutChannel(const ReadoutChannelID& v)
  { hit1ReadoutChannel_ = v; bCalc_ = false; }
  void setHit1Pixel(const PixelID& v) { hit1Pixel_ = v; bCalc_ = false; }
  void setHit1Pixel(int x, int y) { hit1Pixel_.set(x, y); bCalc_ = false; }
  void setHit1Time(double v) { hit1Time_ = v; bCalc_ = false; }
  void setHit1Position(const vector3_t& v) { hit1Position_ = v; bCalc_ = false; }
  void setHit1Position(double x, double y, double z)
  { hit1Position_.set(x, y, z); bCalc_ = false; }
  void setHit1Energy(double v) { hit1Energy_ = v; bCalc_ = false; }

  void setHit2(int hitID, const DetectorHit_sptr& hit);
  void setHit2ID(int v) { hit2ID_ = v; bCalc_ = false; }
  void setHit2Process(uint32_t v) { hit2Process_ = v; bCalc_ = false; }
  void setHit2DetectorChannel(const DetectorChannelID& v)
  { hit2DetectorChannel_ = v; bCalc_ = false; }
  void setHit2ReadoutChannel(const ReadoutChannelID& v)
  { hit2ReadoutChannel_ = v; bCalc_ = false; }
  void setHit2Pixel(const PixelID& v) { hit2Pixel_ = v; bCalc_ = false; }
  void setHit2Pixel(int x, int y) { hit2Pixel_.set(x, y); bCalc_ = false; }
  void setHit2Time(double v) { hit2Time_ = v; bCalc_ = false; }
  void setHit2Position(const vector3_t& v) { hit2Position_ = v; bCalc_ = false; }
  void setHit2Position(double x, double y, double z)
  { hit2Position_.set(x, y, z); bCalc_ = false; }
  void setHit2Energy(double v) { hit2Energy_ = v; bCalc_ = false; }

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

  double CosThetaE() const { refresh(); return cosThetaE_; }
  double ThetaE() const { refresh(); return thetaE_; }
  vector3_t ConeAxis() const { refresh(); return coneAxis_; }
  vector3_t ConeVertex() const { refresh(); return coneVertex_; }
  double ThetaG() const { refresh(); return thetaG_; }
  double DeltaTheta() const { refresh(); return deltaTheta_; }
  double PhiG() const { refresh(); return phiG_; }
  double TotalEnergy()  const { return Hit1Energy()+Hit2Energy(); }
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
  
private:
  void refresh() const { if (!bCalc_) { calc(); } }
  void calc() const;

private:
  int64_t eventID_;
  int numHits_;
  
  int hit1ID_;
  uint32_t hit1Process_;
  DetectorChannelID hit1DetectorChannel_;
  ReadoutChannelID hit1ReadoutChannel_;
  PixelID hit1Pixel_;
  double hit1Time_;
  vector3_t hit1Position_;
  double hit1Energy_;

  int hit2ID_;
  uint32_t hit2Process_;
  DetectorChannelID hit2DetectorChannel_;
  ReadoutChannelID hit2ReadoutChannel_;
  PixelID hit2Pixel_;
  double hit2Time_;
  vector3_t hit2Position_;
  double hit2Energy_;

  uint64_t flags_;

  uint64_t hitpattern_;
  int32_t grade_;
  double likelihood_;

  mutable bool bCalc_;
  mutable double cosThetaE_;
  mutable double thetaE_;
  mutable double thetaG_;
  mutable double deltaTheta_;
  mutable double phiG_;
  mutable vector3_t coneVertex_;
  mutable vector3_t coneAxis_;

  bool sourceDistant_;
  vector3_t sourceDirection_;
  vector3_t sourcePosition_;
};

template <typename T>
bool filter_compton(T (BasicComptonEvent::*getter)() const,
                    const BasicComptonEvent& event,
                    T min, T max)
{
  return min <= (event.*getter)() && (event.*getter)() <= max;
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_BasicComptonEvent_H */
