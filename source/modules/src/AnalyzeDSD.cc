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

#include "AnalyzeDSD.hh"

#include "TH1.h"
#include "TH2.h"

#include "globals.hh"

#include "DetectorHit.hh"
#include "RealDetectorUnit2DStrip.hh"
#include "CSHitCollection.hh"

using namespace comptonsoft;
using namespace anl;

AnalyzeDSD::AnalyzeDSD()
  : m_DSD(0), m_DetID(0), m_Energy0(0.0*keV), m_Energy1(2000.0*keV)
{
}


ANLStatus AnalyzeDSD::mod_startup()
{
  register_parameter(&m_DetID, "Detector ID");
  register_parameter(&m_Energy0, "Energy min", keV, "keV");
  register_parameter(&m_Energy1, "Energy max", keV, "keV");
  return AS_OK;
}


ANLStatus AnalyzeDSD::mod_his()
{
  VCSModule::mod_his();

  const int NBin = 512;
  const double EMin = 0.0;
  const double EMax = 128.0;

  m_Spec1HitCathode = new TH1D("spec_1hit_cathode", "Spectrum (1-hit, cathode)",
			       NBin, EMin, EMax);
  m_Spec1HitAnode = new TH1D("spec_1hit_anode", "Spectrum (1-hit, anode)",
			     NBin, EMin, EMax);
  m_Spec2HitCathode = new TH1D("spec_2hit_cathode", "Spectrum (2-hit, cathode)",
			       NBin, EMin, EMax);
  m_Spec2HitAnode = new TH1D("spec_2hit_anode", "Spectrum (2-hit, anode)",
			     NBin, EMin, EMax);

  m_E2DCathode = new TH2D("e2D_2hit_cathode", "Energy 2D (2-hit, cathode)",
			  NBin, EMin, EMax, NBin, EMin, EMax);
  m_E2DAnode = new TH2D("e2D_2hit_anode", "Energy 2D (2-hit, anode)",
			NBin, EMin, EMax, NBin, EMin, EMax);

  int NPixelX = m_DSD->getNumPixelX();
  int NPixelY = m_DSD->getNumPixelY();
  m_Image = new TH2D("image", "Image",
		     NPixelX, -0.5, NPixelX-0.5,
		     NPixelY, -0.5, NPixelY-0.5);
  m_Image1DX = new TH1D("image_1D_X", "Image 1D X",
			NPixelX, -0.5, NPixelX-0.5);
  m_Image1DY = new TH1D("image_1D_Y", "Image 1D Y",
			NPixelY, -0.5, NPixelY-0.5);

  return AS_OK;
}


ANLStatus AnalyzeDSD::mod_init()
{
  VCSModule::mod_init();

  RealDetectorUnit* det = GetDetectorManager()->getDetectorByID(m_DetID);
  m_DSD = dynamic_cast<RealDetectorUnit2DStrip*>(det);
  if (m_DSD==0) {
    std::cout << "The detector is not 2D-strip." << std::endl;
    return AS_QUIT;
  }

  return AS_OK;
}


ANLStatus AnalyzeDSD::mod_ana()
{
  int numCathode = m_DSD->NumCathodeSideHits();
  int numAnode = m_DSD->NumAnodeSideHits();

  // cathode
  if (numCathode==1) {
    DetectorHit_sptr hit = m_DSD->getCathodeSideHit(0);
    m_Spec1HitCathode->Fill(hit->getPI()/keV);
  }
  else if (numCathode==2) {
    DetectorHit_sptr hit0 = m_DSD->getCathodeSideHit(0);
    DetectorHit_sptr hit1 = m_DSD->getCathodeSideHit(1);
    
    bool adjacent = false;
    int strip0 = -1, strip1 = -1;
    if (hit0->getStripX() >= 0) {
      strip0 = hit0->getStripX();
      strip1 = hit1->getStripX();
      if (strip0+1==strip1 || strip0==strip1+1) adjacent = true;
    }
    else {
      strip0 = hit0->getStripY();
      strip1 = hit1->getStripY();
      if (strip0+1==strip1 || strip0==strip1+1) adjacent = true;
    }

    if (adjacent) {
      double sum = hit0->getPI() + hit1->getPI();
      m_Spec2HitCathode->Fill(sum/keV);
      if (strip0<strip1) {
	m_E2DCathode->Fill(hit0->getPI()/keV, hit1->getPI()/keV);
      }
      else {
	m_E2DCathode->Fill(hit1->getPI()/keV, hit0->getPI()/keV);
      }
    }
  }

  // anode
  if (numAnode==1) {
    DetectorHit_sptr hit = m_DSD->getAnodeSideHit(0);
    m_Spec1HitAnode->Fill(hit->getPI()/keV);
  }
  else if (numAnode==2) {
    DetectorHit_sptr hit0 = m_DSD->getAnodeSideHit(0);
    DetectorHit_sptr hit1 = m_DSD->getAnodeSideHit(1);
    
    bool adjacent = false;
    int strip0 = -1, strip1 = -1;
    if (hit0->getStripX() >= 0) {
      strip0 = hit0->getStripX();
      strip1 = hit1->getStripX();
      if (strip0+1==strip1 || strip0==strip1+1) adjacent = true;
    }
    else {
      strip0 = hit0->getStripY();
      strip1 = hit1->getStripY();
      if (strip0+1==strip1 || strip0==strip1+1) adjacent = true;
    }

    if (adjacent) {
      double sum = hit0->getPI() + hit1->getPI();
      m_Spec2HitAnode->Fill(sum/keV);
      if (strip0<strip1) {
	m_E2DAnode->Fill(hit0->getPI()/keV, hit1->getPI()/keV);
      }
      else {
	m_E2DAnode->Fill(hit1->getPI()/keV, hit0->getPI()/keV);
      }
    }
  }

  // image
  if (numCathode==1 && numAnode==1) {
    DetectorHit_sptr hit0 = m_DSD->getAnodeSideHit(0);
    DetectorHit_sptr hit1 = m_DSD->getCathodeSideHit(0);
    
    int stripX=-1, stripY=-1;
    if (hit0->getStripX() >= 0) {
      stripX = hit0->getStripX();
      stripY = hit1->getStripY();
    }
    else {
      stripX = hit1->getStripX();
      stripY = hit0->getStripY();
    }
    
    double energy = 0.0;
    if (m_DSD->PriorityToAnodeSide()) {
      energy = hit0->getPI();
    }
    else {
      energy = hit1->getPI();
    }

    if (m_Energy0 <= energy && energy <= m_Energy1) {
      m_Image->Fill(stripX, stripY);
      m_Image1DX->Fill(stripX);
      m_Image1DY->Fill(stripY);
    }
  }
  
  return AS_OK;
}
