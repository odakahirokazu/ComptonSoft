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

#include "FillFrame.hh"

#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"
#include "DetectorSystem.hh"
#include "RealDetectorUnit2DPixel.hh"
#include "FrameData.hh"


using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

FillFrame::FillFrame()
{
}

FillFrame::~FillFrame() = default;

ANLStatus FillFrame::mod_define()
{
  define_parameter("offset_x", &mod_class::m_offsetX);
  define_parameter("offset_y", &mod_class::m_offsetY);
  return AS_OK;
}

ANLStatus FillFrame::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &m_HitCollection);

  return AS_OK;
}

ANLStatus FillFrame::mod_analyze()
{
  const std::vector<DetectorHit_sptr>& firstHits = m_HitCollection->getHits(0);
  if (firstHits.size() == 0) { return AS_OK; }

  const DetectorHit_sptr& hit0 = firstHits[0];
  const long frameID = hit0->EventID();

  DetectorSystem* detectorManager = getDetectorManager();

  auto& detectors = detectorManager->getDetectors();
  for (auto& detector: detectors) {
    if (detector->hasFrameData()) {
      FrameData* frame = detector->getFrameData();
      frame->resetRawFrame();
    }
  }

  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    const std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr hit: hits) {
      const int detectorID = hit->DetectorID();
      VRealDetectorUnit* detector = detectorManager->getDetectorByID(detectorID);
      FrameData* frame = detector->getFrameData();
      const int nx = frame->NumPixelsX();
      const int ny = frame->NumPixelsY();
      frame->setFrameID(frameID);
      image_t& rawFrame = frame->getRawFrame();
      const int ix = hit->PixelX() - m_offsetX;
      const int iy = hit->PixelY() - m_offsetY;
      if (ix>=0 && ix<nx && iy>=0 && iy<ny) {
        rawFrame[ix][iy] += hit->EPI()/unit::keV;
      }
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
