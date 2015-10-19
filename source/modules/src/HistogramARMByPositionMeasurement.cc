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

#include "HistogramARMByPositionMeasurement.hh"

#include "G4SystemOfUnits.hh"
#include "TDirectory.h"
#include "TH1.h"
#include "TRandom3.h"
#include "TMath.h"
#include "CSTypes.hh"
#include "BasicComptonEvent.hh"
#include "EventReconstruction.hh"

using namespace anl;

namespace comptonsoft
{

HistogramARMByPositionMeasurement::HistogramARMByPositionMeasurement()
  : m_NumSamples(1000), m_RandomGenerator(new TRandom3)
{
}

HistogramARMByPositionMeasurement::~HistogramARMByPositionMeasurement() = default;

ANLStatus HistogramARMByPositionMeasurement::mod_startup()
{
  HistogramARM::mod_startup();
  register_parameter(&m_NumSamples, "num_samples");
  return AS_OK;
}

ANLStatus HistogramARMByPositionMeasurement::mod_ana()
{
  if (!Evs("EventReconstruction:OK")) {
    return AS_OK;
  }

  const BasicComptonEvent& event = eventReconstruction_->getComptonEvent();
  const double cosThetaE = event.CosThetaE();
  if (cosThetaE < -1.0 || +1.0 < cosThetaE) {
    return AS_OK;
  }

  const int hit1DetectorID = event.Hit1DetectorID();
  const VRealDetectorUnit* hit1Detector = getDetectorManager()->getDetectorByID(hit1DetectorID);
  const vector3_t hit1Position = event.Hit1Position();
  const double hit1HalfWidthX = hit1Detector->getPixelPitchX() * 0.5;
  const double hit1HalfWidthY = hit1Detector->getPixelPitchY() * 0.5;
  const double hit1HalfWidthZ = hit1Detector->getThickness() * 0.5;
  const vector3_t hit1DetectorDirX = hit1Detector->getDirectionX();
  const vector3_t hit1DetectorDirY = hit1Detector->getDirectionY();
  const vector3_t hit1DetectorDirZ = hit1DetectorDirX.cross(hit1DetectorDirY);

  const int hit2DetectorID = event.Hit2DetectorID();
  const VRealDetectorUnit* hit2Detector = getDetectorManager()->getDetectorByID(hit2DetectorID);
  const vector3_t hit2Position = event.Hit2Position();
  const double hit2HalfWidthX = hit2Detector->getPixelPitchX() * 0.5;
  const double hit2HalfWidthY = hit2Detector->getPixelPitchY() * 0.5;
  const double hit2HalfWidthZ = hit2Detector->getThickness() * 0.5;
  const vector3_t hit2DetectorDirX = hit2Detector->getDirectionX();
  const vector3_t hit2DetectorDirY = hit2Detector->getDirectionY();
  const vector3_t hit2DetectorDirZ = hit2DetectorDirX.cross(hit2DetectorDirY);

  const vector3_t sourceDirection = eventReconstruction_->SourceDirection();
  const vector3_t coneAxis = event.ConeAxis();
  const double thetaG = sourceDirection.angle(coneAxis);

  const int NumSamples = m_NumSamples;
  const double FillWeight = 1.0/static_cast<double>(NumSamples);
  TRandom* randgen = m_RandomGenerator.get();

  for (int t=0; t<NumSamples; t++) {
    const double hit1DeltaLocalX = randgen->Uniform(-hit1HalfWidthX, +hit1HalfWidthX);
    const double hit1DeltaLocalY = randgen->Uniform(-hit1HalfWidthY, +hit1HalfWidthY);
    const double hit1DeltaLocalZ = randgen->Uniform(-hit1HalfWidthZ, +hit1HalfWidthZ);
    const double hit2DeltaLocalX = randgen->Uniform(-hit2HalfWidthX, +hit2HalfWidthX);
    const double hit2DeltaLocalY = randgen->Uniform(-hit2HalfWidthY, +hit2HalfWidthY);
    const double hit2DeltaLocalZ = randgen->Uniform(-hit2HalfWidthZ, +hit2HalfWidthZ);

    const vector3_t hit1Position1 = hit1Position
      + hit1DeltaLocalX * hit1DetectorDirX
      + hit1DeltaLocalY * hit1DetectorDirY
      + hit1DeltaLocalZ * hit1DetectorDirZ;
    const vector3_t hit2Position1 = hit2Position
      + hit2DeltaLocalX * hit2DetectorDirX
      + hit2DeltaLocalY * hit2DetectorDirY
      + hit2DeltaLocalZ * hit2DetectorDirZ;

    const vector3_t coneAxis1 = hit1Position1 - hit2Position1;
    const double thetaG1 = sourceDirection.angle(coneAxis1);
    const double ARMValueByPosition = (thetaG1 - thetaG)/degree;
    hist_all_->Fill(ARMValueByPosition, FillWeight);
    for (std::size_t i=0; i<hist_vec_.size(); i++) {
      if (eventReconstruction_->HitPatternFlag(i)) {
        hist_vec_[i]->Fill(ARMValueByPosition, FillWeight);
      }
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
