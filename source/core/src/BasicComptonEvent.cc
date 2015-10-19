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

#include "BasicComptonEvent.hh"
#include <utility>
#include <cmath>
#include "AstroUnits.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

BasicComptonEvent::BasicComptonEvent()
  : eventID_(0), numHits_(0),
    hit1ID_(0), hit1Process_(0),
    hit1Pixel_(0, 0), hit1Time_(0.0), hit1Position_(0.0, 0.0, 0.0),
    hit1Energy_(0.0),
    hit2ID_(0), hit2Process_(0),
    hit2Pixel_(0, 0), hit2Time_(0.0), hit2Position_(0.0, 0.0, 0.0),
    hit2Energy_(0.0),
    flags_(0ul), hitpattern_(0ul),
    grade_(0), likelihood_(1.0),
    bCalc_(false),
    cosThetaE_(1.0), thetaE_(0.0), thetaG_(0.0), deltaTheta_(0.0),
    phiG_(0.0),
    coneVertex_(0.0, 0.0, 0.0), coneAxis_(0.0, 0.0, 1.0),
    sourceDistant_(true), sourceDirection_(0.0, 0.0, 1.0),
    sourcePosition_(0.0, 0.0, 0.0)
{
}

BasicComptonEvent::~BasicComptonEvent() = default;

void BasicComptonEvent::setHit1(int hitID, const DetectorHit_sptr& hit)
{
  hit1ID_ = hitID;
  hit1Process_ = hit->Process();
  hit1DetectorChannel_ = hit->DetectorChannel();
  hit1ReadoutChannel_ = hit->ReadoutChannel();
  hit1Pixel_ = hit->Pixel();
  hit1Time_ = hit->Time();
  hit1Position_ = hit->Position();
  hit1Energy_ = hit->Energy();
  bCalc_ = false;
}

void BasicComptonEvent::setHit2(int hitID, const DetectorHit_sptr& hit)
{
  hit2ID_ = hitID;
  hit2Process_ = hit->Process();
  hit2DetectorChannel_ = hit->DetectorChannel();
  hit2ReadoutChannel_ = hit->ReadoutChannel();
  hit2Pixel_ = hit->Pixel();
  hit2Time_ = hit->Time();
  hit2Position_ = hit->Position();
  hit2Energy_ = hit->Energy();
  bCalc_ = false;
}

vector3_t BasicComptonEvent::SourceDirection() const 
{ 
  if (!SourceDistant()) {
    return (SourcePosition()-ConeVertex()).unit();
  }
  return sourceDirection_;
}

void BasicComptonEvent::swap()
{
  bCalc_ = false;
  std::swap(hit1ID_, hit2ID_);
  std::swap(hit1Process_, hit2Process_);
  std::swap(hit1DetectorChannel_, hit2DetectorChannel_);
  std::swap(hit1ReadoutChannel_, hit2ReadoutChannel_);
  std::swap(hit1Pixel_, hit2Pixel_);
  std::swap(hit1Time_, hit2Time_);
  std::swap(hit1Position_, hit2Position_);
  std::swap(hit1Energy_, hit2Energy_);
}

double BasicComptonEvent::DistanceBetweenTheHits() const
{
  return (Hit1Position()-Hit2Position()).mag();
}

void BasicComptonEvent::calc() const
{
  using std::acos;
  using std::atan2;
  using std::floor;

  cosThetaE_ = 1.0 - (electron_mass_c2*hit1Energy_)/(hit2Energy_*(hit1Energy_+hit2Energy_));
  const double k = floor(cosThetaE_/2.0+0.5);
  thetaE_ = acos(cosThetaE_ - 2.0*k) - pi*k;

  coneVertex_ = hit1Position_;
  const vector3_t scatteringVector = (hit2Position_-hit1Position_).unit();
  coneAxis_ = -scatteringVector;

  const vector3_t direction = SourceDirection();
  thetaG_ = direction.angle(coneAxis_);
  deltaTheta_ = thetaE_ - thetaG_;

  const vector3_t xaxis(1.0, 0.0, 0.0);
  const vector3_t axis1 = (xaxis-(direction.dot(xaxis))*direction).unit();
  const vector3_t axis2 = direction.cross(axis1);
  const double s1 = scatteringVector.dot(axis1);
  const double s2 = scatteringVector.dot(axis2);
  phiG_ = atan2(s2, s1);
  
  bCalc_ = true;
}

} /* namespace comptonsoft */
