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

#include "QuickAnalysisForDSD.hh"

#include "TH1.h"
#include "TH2.h"
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "RealDetectorUnit2DStrip.hh"

using namespace anl;

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

QuickAnalysisForDSD::QuickAnalysisForDSD()
  : m_DSD(nullptr), m_DetectorID(0),
    m_NBins(2048), m_Energy0(0.0), m_Energy1(204.8),
    m_CutEnergy0(0.0*unit::keV), m_CutEnergy1(204.8*unit::keV)
{
}

QuickAnalysisForDSD::~QuickAnalysisForDSD() = default;

ANLStatus QuickAnalysisForDSD::mod_define()
{
  register_parameter(&m_DetectorID, "detector_id");
  register_parameter(&m_NBins, "num_bins");
  register_parameter(&m_Energy0, "energy_min", 1, "keV");
  register_parameter(&m_Energy1, "energy_max", 1, "keV");
  register_parameter(&m_CutEnergy0, "cut_energy_min", unit::keV, "keV");
  register_parameter(&m_CutEnergy1, "cut_energy_max", unit::keV, "keV");
  return AS_OK;
}

ANLStatus QuickAnalysisForDSD::mod_initialize()
{
  VCSModule::mod_initialize();

  const int NBin = m_NBins;
  const double EMin = m_Energy0;
  const double EMax = m_Energy1;

  m_Spectrum1HitCathode = new TH1D("spectrum_1hit_cathode", "Spectrum (1-hit, cathode)",
                                   NBin, EMin, EMax);
  m_Spectrum1HitAnode = new TH1D("spectrum_1hit_anode", "Spectrum (1-hit, anode)",
                                 NBin, EMin, EMax);
  m_Spectrum2HitCathode = new TH1D("spectrum_2hit_cathode", "Spectrum (2-hit, cathode)",
                                   NBin, EMin, EMax);
  m_Spectrum2HitAnode = new TH1D("spectrum_2hit_anode", "Spectrum (2-hit, anode)",
                                 NBin, EMin, EMax);

  m_E2D2HitCathode = new TH2D("e2D_2hit_cathode", "Energy 2D (2-hit, cathode)",
                              NBin, EMin, EMax, NBin, EMin, EMax);
  m_E2D2HitAnode = new TH2D("e2D_2hit_anode", "Energy 2D (2-hit, anode)",
                            NBin, EMin, EMax, NBin, EMin, EMax);
  m_E2DDoubleSide = new TH2D("e2D_1hit_double", "Energy 2D (1-hit, cathode/anode)",
                             NBin, EMin, EMax, NBin, EMin, EMax);

  const int NPixelX = m_DSD->getNumPixelX();
  const int NPixelY = m_DSD->getNumPixelY();
  m_Image = new TH2D("image", "Image",
                     NPixelX, -0.5, NPixelX-0.5,
                     NPixelY, -0.5, NPixelY-0.5);
  m_Image1DX = new TH1D("image_1D_X", "Image 1D X",
                        NPixelX, -0.5, NPixelX-0.5);
  m_Image1DY = new TH1D("image_1D_Y", "Image 1D Y",
                        NPixelY, -0.5, NPixelY-0.5);

  VRealDetectorUnit* det = getDetectorManager()->getDetectorByID(m_DetectorID);
  m_DSD = dynamic_cast<RealDetectorUnit2DStrip*>(det);
  if (m_DSD==nullptr) {
    std::cout << "The detector is not 2D-strip." << std::endl;
    return AS_QUIT;
  }

  return AS_OK;
}

ANLStatus QuickAnalysisForDSD::mod_analyze()
{
  const int numCathode = m_DSD->NumberOfCathodeSideHits();
  const int numAnode = m_DSD->NumberOfAnodeSideHits();

  // cathode
  if (numCathode==1) {
    DetectorHit_sptr hit = m_DSD->getCathodeSideHit(0);
    m_Spectrum1HitCathode->Fill(hit->EPI()/unit::keV);
  }
  else if (numCathode==2) {
    DetectorHit_sptr hit0 = m_DSD->getCathodeSideHit(0);
    DetectorHit_sptr hit1 = m_DSD->getCathodeSideHit(1);
    
    bool adjacent = false;
    int strip0 = -1, strip1 = -1;
    if (hit0->isXStrip()) {
      strip0 = hit0->PixelX();
      strip1 = hit1->PixelX();
      if (strip0+1==strip1 || strip0==strip1+1) adjacent = true;
    }
    else {
      strip0 = hit0->PixelY();
      strip1 = hit1->PixelY();
      if (strip0+1==strip1 || strip0==strip1+1) adjacent = true;
    }

    if (adjacent) {
      double sum = hit0->EPI() + hit1->EPI();
      m_Spectrum2HitCathode->Fill(sum/unit::keV);
      if (strip0<strip1) {
        m_E2D2HitCathode->Fill(hit0->EPI()/unit::keV, hit1->EPI()/unit::keV);
      }
      else {
        m_E2D2HitCathode->Fill(hit1->EPI()/unit::keV, hit0->EPI()/unit::keV);
      }
    }
  }

  // anode
  if (numAnode==1) {
    DetectorHit_sptr hit = m_DSD->getAnodeSideHit(0);
    m_Spectrum1HitAnode->Fill(hit->EPI()/unit::keV);
  }
  else if (numAnode==2) {
    DetectorHit_sptr hit0 = m_DSD->getAnodeSideHit(0);
    DetectorHit_sptr hit1 = m_DSD->getAnodeSideHit(1);
    
    bool adjacent = false;
    int strip0 = -1, strip1 = -1;
    if (hit0->isXStrip()) {
      strip0 = hit0->PixelX();
      strip1 = hit1->PixelX();
      if (strip0+1==strip1 || strip0==strip1+1) adjacent = true;
    }
    else {
      strip0 = hit0->PixelY();
      strip1 = hit1->PixelY();
      if (strip0+1==strip1 || strip0==strip1+1) adjacent = true;
    }

    if (adjacent) {
      double sum = hit0->EPI() + hit1->EPI();
      m_Spectrum2HitAnode->Fill(sum/unit::keV);
      if (strip0<strip1) {
        m_E2D2HitAnode->Fill(hit0->EPI()/unit::keV, hit1->EPI()/unit::keV);
      }
      else {
        m_E2D2HitAnode->Fill(hit1->EPI()/unit::keV, hit0->EPI()/unit::keV);
      }
    }
  }

  // both side 1-hit
  if (numCathode==1 && numAnode==1) {
    DetectorHit_sptr hitAnode = m_DSD->getAnodeSideHit(0);
    DetectorHit_sptr hitCathode = m_DSD->getCathodeSideHit(0);
    m_E2DDoubleSide->Fill(hitAnode->EPI()/unit::keV, hitCathode->EPI()/unit::keV);
  }

  // image
  if (numCathode==1 && numAnode==1) {
    DetectorHit_sptr hit0 = m_DSD->getAnodeSideHit(0);
    DetectorHit_sptr hit1 = m_DSD->getCathodeSideHit(0);
    
    int stripX=-1, stripY=-1;
    if (hit0->PixelX() >= 0) {
      stripX = hit0->PixelX();
      stripY = hit1->PixelY();
    }
    else {
      stripX = hit1->PixelX();
      stripY = hit0->PixelY();
    }
    
    double energy = 0.0;
    if (m_DSD->PriorityToAnodeSide()) {
      energy = hit0->EPI();
    }
    else {
      energy = hit1->EPI();
    }

    if (m_CutEnergy0 <= energy && energy <= m_CutEnergy1) {
      m_Image->Fill(stripX, stripY);
      m_Image1DX->Fill(stripX);
      m_Image1DY->Fill(stripY);
    }
  }
  
  return AS_OK;
}

} /* namespace comptonsoft */
