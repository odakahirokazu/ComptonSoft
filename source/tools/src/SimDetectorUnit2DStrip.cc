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

// SimDetectorUnit2DStrip.cc
//
// 2008-08-25 Hirokazu Odaka
// 2008-07-11 Hirokazu Odaka

#include "SimDetectorUnit2DStrip.hh"

#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include "TMath.h"
#include "TRandom3.h"

#include "HXISGDFlagDef.hh"
#include "DetectorHit.hh"
#include "CalcWPStrip.hh"

namespace comptonsoft {

SimDetectorUnit2DStrip::SimDetectorUnit2DStrip()
  : m_UpsideXStrip(false),
    m_UseSymmetry(true),
    m_CCEMapXStrip(0),
    m_CCEMapYStrip(0),
    m_WPMapXStrip(0),
    m_WPMapYStrip(0)
{
  registerDetectorUnit( static_cast<RealDetectorUnit*>(this) );
  setNumPixelInWPCalculation(20.0);
}


SimDetectorUnit2DStrip::~SimDetectorUnit2DStrip()
{
}


void SimDetectorUnit2DStrip::setThresholdEnergyCathode(double val)
{
  if (UpsideXStrip()==UpsideCathode()) {
    setThresholdEnergyX(val);
  }
  else {
    setThresholdEnergyY(val);
  }
}


void SimDetectorUnit2DStrip::setThresholdEnergyAnode(double val)
{
  if (UpsideXStrip()==UpsideAnode()) {
    setThresholdEnergyX(val);
  }
  else {
    setThresholdEnergyY(val);
  }
}


double SimDetectorUnit2DStrip::ThresholdEnergyAnode()
{
  if (UpsideXStrip() == UpsideAnode()) {
    return ThresholdEnergyX();
  }
  else {
    return ThresholdEnergyY();
  }
}


double SimDetectorUnit2DStrip::ThresholdEnergyCathode()
{
  if (UpsideXStrip() == UpsideCathode()) {
    return ThresholdEnergyX();
  }
  else {
    return ThresholdEnergyY();
  }
}


void SimDetectorUnit2DStrip::simulatePulseHeights()
{
  std::vector<DetectorHit_sptr>::iterator it;
  std::vector<DetectorHit_sptr>::iterator itEnd = m_RawHits.end();

  for (it = m_RawHits.begin(); it != itEnd; it++) {
    DetectorHit_sptr rawhit = *it;
    double edep = (*it)->getEdep();
    double localposx = (*it)->getLocalPosX();
    double localposy = (*it)->getLocalPosY();
    double localposz = (*it)->getLocalPosZ();
    StripPair sp = StripXYByLocalPosition(localposx, localposy);
    
    if (edep == 0.0) {
      DetectorHit_sptr xhit(new DetectorHit(*rawhit));
      xhit->setStrip(sp.X(), comptonsoft::NO_STRIP);
      if (UpsideXStrip()==UpsideAnode()) {
        xhit->addFlag(ANODE_SIDE);
        xhit->addFlag(PriorityToAnodeSide() ? PRIORITY_SIDE : 0);
      }
      else {
        xhit->addFlag(PriorityToCathodeSide() ? PRIORITY_SIDE : 0);
      }
      
      DetectorHit_sptr yhit(new DetectorHit(*rawhit));
      yhit->setStrip(comptonsoft::NO_STRIP, sp.Y());
      yhit->addFlag(PRIORITY_SIDE);
      if (UpsideYStrip()==UpsideAnode()) {
        yhit->addFlag(ANODE_SIDE);
        yhit->addFlag(PriorityToAnodeSide() ? PRIORITY_SIDE : 0);
      }
      else {
        yhit->addFlag(PriorityToCathodeSide() ? PRIORITY_SIDE : 0);
      }
  
      insertSimulatedHit(xhit);
      insertSimulatedHit(yhit);
      continue;
    }

    if (SimDiffusionMode()==0) {
      StripPair xsp(sp.X(), comptonsoft::NO_STRIP);
      StripPair ysp(comptonsoft::NO_STRIP, sp.Y());
      DetectorHit_sptr xhit = generateHit(*rawhit, xsp);
      DetectorHit_sptr yhit = generateHit(*rawhit, ysp);
      insertSimulatedHit(xhit);
      insertSimulatedHit(yhit);
    }
    else {
      StripPair xsp(sp.X(), comptonsoft::NO_STRIP);
      StripPair ysp(comptonsoft::NO_STRIP, sp.Y());
      DetectorHit_sptr xhit = generateHit(*rawhit, xsp);
      DetectorHit_sptr yhit = generateHit(*rawhit, ysp);
      double xpha = xhit->getPHA();
      double ypha = yhit->getPHA();

      int numDivision = DiffusionDivisionNumber();
      double edepDivision = edep/numDivision;
      double xphaDivision = xpha/numDivision;
      double yphaDivision = ypha/numDivision;
      
      double xDiffusionSigma = 0.0;
      double yDiffusionSigma = 0.0;
      if (UpsideXStrip() == UpsideAnode()) {
        // x-side is anode
        xDiffusionSigma = DiffusionSigmaAnode(localposz);
        yDiffusionSigma = DiffusionSigmaCathode(localposz);
      }
      else {
        // x-side is cathode
        xDiffusionSigma = DiffusionSigmaCathode(localposz);
        yDiffusionSigma = DiffusionSigmaAnode(localposz);
      }

      std::vector<DetectorHit_sptr> diffusionHits;
      std::vector<DetectorHit_sptr>::iterator itHit;

      // x-strip
      for (int l=0; l<numDivision; l++) {
        double radiusDiffusion = gRandom->Gaus(0.0, xDiffusionSigma);
        double thetaDiffusion = gRandom->Uniform(TMath::TwoPi());
        double dx = radiusDiffusion * TMath::Cos(thetaDiffusion);
        double dy = radiusDiffusion * TMath::Sin(thetaDiffusion);
        StripPair spDiff = StripXYByLocalPosition(localposx+dx, localposy+dy);
        spDiff.setY(comptonsoft::NO_STRIP);

        using namespace boost::lambda;
        itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                        bind(&DetectorHit::getStrip, *_1) == spDiff);
        
        if (itHit != diffusionHits.end()) {
          (*itHit)->setEdep( (*itHit)->getEdep() + edepDivision );
          (*itHit)->setPHA( (*itHit)->getPHA() + xphaDivision );
        }
        else {
          DetectorHit_sptr hitDivision(new DetectorHit(*xhit));
          hitDivision->setEdep(edepDivision);
          hitDivision->setPHA(xphaDivision);
          hitDivision->setStrip(spDiff);
          diffusionHits.push_back(hitDivision);
        }
      }

      for (itHit=diffusionHits.begin(); itHit!=diffusionHits.end(); itHit++) {
        insertSimulatedHit(*itHit);
      }

      diffusionHits.clear();

      // y-strip
      for (int l=0; l<numDivision; l++) {
        double radiusDiffusion = gRandom->Gaus(0.0, yDiffusionSigma);
        double thetaDiffusion = gRandom->Uniform(TMath::TwoPi());
        double dx = radiusDiffusion * TMath::Cos(thetaDiffusion);
        double dy = radiusDiffusion * TMath::Sin(thetaDiffusion);
        StripPair spDiff = StripXYByLocalPosition(localposx+dx, localposy+dy);
        spDiff.setX(comptonsoft::NO_STRIP);
        
        using namespace boost::lambda;
        itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                        bind(&DetectorHit::getStrip, *_1) == spDiff);
        
        if (itHit != diffusionHits.end()) {
          (*itHit)->setEdep( (*itHit)->getEdep() + edepDivision );
          (*itHit)->setPHA( (*itHit)->getPHA() + yphaDivision );
        }
        else {
          DetectorHit_sptr hitDivision(new DetectorHit(*yhit));
          hitDivision->setEdep(edepDivision);
          hitDivision->setPHA(yphaDivision);
          hitDivision->setStrip(spDiff);
          diffusionHits.push_back(hitDivision);
        }
      }

      for (itHit=diffusionHits.begin(); itHit!=diffusionHits.end(); itHit++) {
        insertSimulatedHit(*itHit);
      }
    }
      
    // near strips
    if (SimPHAMode()==3) {
      DetectorHit_sptr hit;
      StripPair nearStrip;
     
      nearStrip.set(sp.X()+2, comptonsoft::NO_STRIP);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEdep(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()+1, comptonsoft::NO_STRIP);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEdep(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()-1, comptonsoft::NO_STRIP);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEdep(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()-2, comptonsoft::NO_STRIP);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEdep(0.0);
      insertSimulatedHit(hit);
    
      nearStrip.set(comptonsoft::NO_STRIP, sp.Y()+2);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEdep(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(comptonsoft::NO_STRIP, sp.Y()+1);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEdep(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(comptonsoft::NO_STRIP, sp.Y()-1);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEdep(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(comptonsoft::NO_STRIP, sp.Y()-2);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEdep(0.0);
      insertSimulatedHit(hit);   
    }
  }
}


DetectorHit_sptr SimDetectorUnit2DStrip::generateHit(const DetectorHit& rawhit,
                                                     const StripPair& sp)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  if (sp.X()>=0) {
    if (UpsideXStrip()==UpsideAnode()) {
      hit->addFlag(ANODE_SIDE);
      hit->addFlag(PriorityToAnodeSide() ? PRIORITY_SIDE : 0);
    }
    else {
      hit->addFlag(PriorityToCathodeSide() ? PRIORITY_SIDE : 0);
    }
  }
  else {
    if (UpsideYStrip()==UpsideAnode()) {
      hit->addFlag(ANODE_SIDE);
      hit->addFlag(PriorityToAnodeSide() ? PRIORITY_SIDE : 0);
    }
    else {
      hit->addFlag(PriorityToCathodeSide() ? PRIORITY_SIDE : 0);
    }
  }
  
  if (RangeCheck(sp)) {
    hit->setStrip(sp);
  }
  else {
    hit->setStrip(comptonsoft::GUARD_RING, comptonsoft::GUARD_RING);
    return hit;
  }

  double edep = hit->getEdep();
  double localposx = hit->getLocalPosX();
  double localposy = hit->getLocalPosY();
  double localposz = hit->getLocalPosZ();

  if (hit->isProcess(comptonsoft::QUENCHING)) {
    edep *= QuenchingFactor();
  }

  double pha = calculatePHA(sp, edep, localposx, localposy, localposz);
  hit->setPHA(pha);
  
  return hit;
}


void SimDetectorUnit2DStrip::setBadChannelXStripSideAll(int val)
{
  const int numStrip = getNumPixelX();
  for (int i=0; i<numStrip; i++) {
    setBadChannel(StripPair(i, comptonsoft::NO_STRIP), val);
  }
}


void SimDetectorUnit2DStrip::setBadChannelYStripSideAll(int val)
{
  const int numStrip = getNumPixelY();
  for (int i=0; i<numStrip; i++) {
    setBadChannel(StripPair(comptonsoft::NO_STRIP, i), val);
  }
}


void SimDetectorUnit2DStrip::setNoiseFWHMXStripSideAll(double param0, double param1, double param2)
{
  const int numStrip = getNumPixelX();
  for (int i=0; i<numStrip; i++) {
    setNoiseFWHM(StripPair(i, comptonsoft::NO_STRIP), param0, param1, param2);
  }
}


void SimDetectorUnit2DStrip::setNoiseFWHMYStripSideAll(double param0, double param1, double param2)
{
  const int numStrip = getNumPixelY();
  for (int i=0; i<numStrip; i++) {
    setNoiseFWHM(StripPair(comptonsoft::NO_STRIP, i), param0, param1, param2);
  }
}


void SimDetectorUnit2DStrip::setGainCorrectionFunctionXStripSideAll(const TSpline* val)
{
  const int numStrip = getNumPixelX();
  for (int i=0; i<numStrip; i++) {
    setGainCorrectionFunction(StripPair(i, comptonsoft::NO_STRIP), val);
  }
}


void SimDetectorUnit2DStrip::setGainCorrectionFunctionYStripSideAll(const TSpline* val)
{
  const int numStrip = getNumPixelY();
  for (int i=0; i<numStrip; i++) {
    setGainCorrectionFunction(StripPair(comptonsoft::NO_STRIP, i), val);
  }
}


void SimDetectorUnit2DStrip::setGainCorrectionFactorXStripSideAll(double val)
{
  const int numStrip = getNumPixelX();
  for (int i=0; i<numStrip; i++) {
    setGainCorrectionFactor(StripPair(i, comptonsoft::NO_STRIP), val);
  }
}


void SimDetectorUnit2DStrip::setGainCorrectionFactorYStripSideAll(double val)
{
  const int numStrip = getNumPixelY();
  for (int i=0; i<numStrip; i++) {
    setGainCorrectionFactor(StripPair(comptonsoft::NO_STRIP, i), val);
  }
}



double SimDetectorUnit2DStrip::ChargeCollectionEfficiency(const StripPair& sp, double x, double y, double z)
{
  double xPixel, yPixel, zPixel;
  LocalPosition(sp, &xPixel, &yPixel, &zPixel);
  
  double cce;
  if (sp.Y()==comptonsoft::NO_STRIP) {
  double xInPixel = x - xPixel;
    int ix = m_CCEMapXStrip->GetXaxis()->FindBin(xInPixel);
    int iz = m_CCEMapXStrip->GetYaxis()->FindBin(z);
    cce =  m_CCEMapXStrip->GetBinContent(ix, iz);
  }
  else {
    double yInPixel = y - yPixel;
    int iy = m_CCEMapYStrip->GetXaxis()->FindBin(yInPixel);
    int iz = m_CCEMapYStrip->GetYaxis()->FindBin(z);
    cce =  m_CCEMapYStrip->GetBinContent(iy, iz);
  }

  return cce;
}


double SimDetectorUnit2DStrip::WeightingPotential(const StripPair& sp, double x, double y, double z)
{
  double xPixel, yPixel, zPixel;
  Position(sp, &xPixel, &yPixel, &zPixel);
  
  double xInPixel = x - xPixel;
  double yInPixel = y - yPixel;

  double wp;
  if (sp.X()!=comptonsoft::NO_STRIP) {
    int ix = m_WPMapXStrip->GetXaxis()->FindBin(xInPixel);
    int iz = m_WPMapXStrip->GetYaxis()->FindBin(z);
    wp =  m_WPMapXStrip->GetBinContent(ix, iz);
  }
  else {
    int iy = m_WPMapYStrip->GetXaxis()->FindBin(yInPixel);
    int iz = m_WPMapYStrip->GetYaxis()->FindBin(z);
    wp =  m_WPMapYStrip->GetBinContent(iy, iz);
  }

  return wp;
}


bool SimDetectorUnit2DStrip::isCCEMapPrepared()
{
  return (getCCEMapXStrip()!=0 && getCCEMapYStrip()!=0);
}


void SimDetectorUnit2DStrip::buildWPMap()
{
  if (SimPHAMode() == 2) {
    buildWPMap(19, 19, 128, 1.0);
  }
  else if (SimPHAMode() == 3) {
    buildWPMap(95, 95, 128, 5.0);
  }
  else {
    std::cout << "Error : buildWPMap() " << std::endl;
    return;
  }
}


void SimDetectorUnit2DStrip::buildWPMap(int nx, int ny, int nz, double pixel_factor)
{
  if (nx<1 || ny<1 || nz<=1) {
    std::cout << "Error : buildWPMap " << std::endl;
    return;
  }

  const double MapSizeX = getPixelPitchX()*pixel_factor;
  const double MapSizeY = getPixelPitchY()*pixel_factor;
  const double MapSizeZ = getThickness()*static_cast<double>(nz)/static_cast<double>(nz-1);

  if (SimPHAMode() == 2) {
    if (MapSizeX<getPixelPitchX()*0.999 || MapSizeY<getPixelPitchY()*0.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 2.";
    }
  }
  else if (SimPHAMode() == 3) {
    if (MapSizeX<getPixelPitchX()*4.999 || MapSizeY<getPixelPitchY()*4.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 3.";
    }
  }
  
  char histname[256];
  sprintf(histname, "wp_x_%04d", getID());
  m_WPMapXStrip =  new TH2D(histname, histname,
                            nx, -0.5*MapSizeX, +0.5*MapSizeX,
                            nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
  sprintf(histname, "wp_y_%04d", getID());
  m_WPMapYStrip =  new TH2D(histname, histname,
                            ny, -0.5*MapSizeY, +0.5*MapSizeY,
                            nz, -0.5*MapSizeZ, +0.5*MapSizeZ);

  std::cout << "calculating weighing potential..." << std::endl;

  double x, y, z;
  boost::scoped_ptr<CalcWPStrip> calc(new CalcWPStrip);
  calc->set_geometry(getPixelPitchX(), getThickness(),
                           NumPixelInWPCalculation());
  calc->init_table();
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    x = m_WPMapXStrip->GetXaxis()->GetBinCenter(ix);
    calc->set_x(x);
    for (int iz=1; iz<=nz; iz++) {
      z = m_WPMapXStrip->GetYaxis()->GetBinCenter(iz);
      if (UpsideXStrip()) {
        double wp = 0.;
        if (iz==nz) {
          if (TMath::Abs(x)<0.5*getPixelPitchX()) {
            wp = 1.;
          }
          else {
            wp = 0.;
          }
        }
        else if (iz==1) {
          wp = 0.;
        }
        else {
          wp = calc->weighting_potential(z);
        }
        m_WPMapXStrip->SetBinContent(ix, iz, wp);
      }
      else {
        double wp = 0.;
        if (iz==1) {
          if (TMath::Abs(x)<0.5*getPixelPitchX()) {
            wp = 1.;
          }
          else {
            wp = 0.;
          }
        }
        else if (iz==nz) {
          wp = 0.;
        }
        else {
          wp = calc->weighting_potential(-z);
        }
        m_WPMapXStrip->SetBinContent(ix, iz, wp);
      }
    }
  }

  calc->set_geometry(getPixelPitchY(), getThickness(),
                           NumPixelInWPCalculation());
  calc->init_table();
  for (int iy=1; iy<=ny; iy++) {
    std::cout << '*' << std::flush;
    y = m_WPMapYStrip->GetXaxis()->GetBinCenter(iy);
    calc->set_x(y);
    for (int iz=1; iz<=nz; iz++) {
      z = m_WPMapYStrip->GetYaxis()->GetBinCenter(iz);
      if (UpsideYStrip()) {
        double wp = 0.;
        if (iz==nz) {
          if (TMath::Abs(y)<0.5*getPixelPitchY()) {
            wp = 1.;
          }
          else {
            wp = 0.;
          }
        }
        else if (iz==1) {
          wp = 0.;
        }
        else {
          wp = calc->weighting_potential(z);
        }
        m_WPMapYStrip->SetBinContent(iy, iz, wp);
      }
      else {
        double wp = 0.;
        if (iz==1) {
          if (TMath::Abs(y)<0.5*getPixelPitchY()) {
            wp = 1.;
          }
          else {
            wp = 0.;
          }
        }
        else if (iz==nz) {
          wp = 0.;
        }
        else {
          wp = calc->weighting_potential(-z);
        }
        m_WPMapYStrip->SetBinContent(iy, iz, wp);
      }
    }
  }
  
  std::cout << std::endl;
}


void SimDetectorUnit2DStrip::buildCCEMap()
{
  if (SimPHAMode() == 2) {
    buildCCEMap(19, 19, 127, 1.0);
  }
  else if (SimPHAMode() == 3) {
    buildCCEMap(95, 95, 127, 5.0);
  }
  else {
    std::cout << "Error : buildCCEMap() " << std::endl;
    return;
  }
}


void SimDetectorUnit2DStrip::buildCCEMap(int nx, int ny, int nz, double pixel_factor)
{
  if (nx<1 || ny<1 || nz<1) {
    std::cout << "Error : buildCCEMap " << std::endl;
    return;
  }

  const double MapSizeX = getPixelPitchX()*pixel_factor;
  const double MapSizeY = getPixelPitchY()*pixel_factor;
  const double MapSizeZ = getThickness();

  if (SimPHAMode() == 2) {
    if (MapSizeX<getPixelPitchX()*0.999 || MapSizeY<getPixelPitchY()*0.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 2.";
    }
  }
  else if (SimPHAMode() == 3) {
    if (MapSizeX<getPixelPitchX()*2.999 || MapSizeY<getPixelPitchY()*2.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 3.";
    }
  }
  
  char histname[256];
  sprintf(histname, "cce_x_%04d", getID());
  m_CCEMapXStrip =  new TH2D(histname, histname,
                             nx, -0.5*MapSizeX, +0.5*MapSizeX,
                             nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
  sprintf(histname, "cce_y_%04d", getID());
  m_CCEMapYStrip =  new TH2D(histname, histname,
                             nx, -0.5*MapSizeY, +0.5*MapSizeY,
                             nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
  
  std::cout << "calculating charge collection efficiency..." << std::endl;
  
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    if (!m_UseSymmetry || ix<=(nx+1)/2) {
      double x = m_CCEMapXStrip->GetXaxis()->GetBinCenter(ix);
      int binx = m_WPMapXStrip->GetXaxis()->FindBin(x);
      
      int numPoints = nz+1;
      boost::shared_array<double> wp(new double[numPoints]);
      for (int k=0; k<numPoints; k++) {
        double z = m_CCEMapXStrip->GetYaxis()->GetBinLowEdge(k+1);
        int binz = m_WPMapXStrip->GetYaxis()->FindBin(z);
        wp[k] = m_WPMapXStrip->GetBinContent(binx, binz);
      }
      setWPForCCECalculation(UpsideXStrip(), wp, numPoints);
      
      for (int iz=1; iz<=nz; iz++) {
        double z = m_CCEMapXStrip->GetYaxis()->GetBinCenter(iz);
        double cce = calculateCCE(z);
        // std::cout << cce << std::endl;
        m_CCEMapXStrip->SetBinContent(ix, iz, cce);
      }
    }
  }

  if (m_UseSymmetry) {
    for (int ix=1; ix<=nx; ix++) {
      for (int iz=1; iz<=nz; iz++) {
        if (ix>(nx+1)/2) {
          double cce = m_CCEMapXStrip->GetBinContent(nx-ix+1, iz);
          m_CCEMapXStrip->SetBinContent(ix, iz, cce);
        }
      }
    }
  }

  for (int iy=1; iy<=ny; iy++) {
    std::cout << '*' << std::flush;
    if (!m_UseSymmetry || iy<=(ny+1)/2) {
      double y = m_CCEMapYStrip->GetXaxis()->GetBinCenter(iy);
      int biny = m_WPMapYStrip->GetXaxis()->FindBin(y);
      
      int numPoints = nz+1;
      boost::shared_array<double> wp(new double[numPoints]);
      for (int k=0; k<numPoints; k++) {
        double z = m_CCEMapYStrip->GetYaxis()->GetBinLowEdge(k+1);
        int binz = m_WPMapYStrip->GetYaxis()->FindBin(z);
        wp[k] = m_WPMapYStrip->GetBinContent(biny, binz);
      }
      setWPForCCECalculation(UpsideYStrip(), wp, numPoints);
      
      for (int iz=1; iz<=nz; iz++) {
        double z = m_CCEMapYStrip->GetYaxis()->GetBinCenter(iz);
        double cce = calculateCCE(z);
        m_CCEMapYStrip->SetBinContent(iy, iz, cce);
      }
    }
  }

  if (m_UseSymmetry) {
    for (int iy=1; iy<=ny; iy++) {
      for (int iz=1; iz<=nz; iz++) {
        if (iy>(ny+1)/2) {
          double cce = m_CCEMapYStrip->GetBinContent(ny-iy+1, iz);
          m_CCEMapYStrip->SetBinContent(iy, iz, cce);
        }
      }
    }
  }

  std::cout << std::endl;
  std::cout << "Charge collection efficiency map is built." << std::endl;
}

void SimDetectorUnit2DStrip::printSimParam()
{
  std::cout << "SimDetectorUnit2DStrip:" << '\n'
            << " upside xstrip : " << UpsideXStrip() << '\n'
            << std::endl;
  DeviceSimulation::printSimParam();
}

}

