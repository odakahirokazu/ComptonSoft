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

// SimDetectorUnit2DPad.cc
//
// 2008-08-25 Hirokazu Odaka
// 2009-05-22 Hirokazu Odaka
// 2009-05-30 Hirokazu Odaka
// 2009-06-01 Hirokazu Odaka
// 2009-07-09 Hirokazu Odaka

#include "SimDetectorUnit2DPad.hh"

#include <iostream>
#include <algorithm>
#include <boost/scoped_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include "TMath.h"
#include "TRandom3.h"
#include "TH3.h"

#include "HXISGDFlagDef.hh"
#include "DetectorHit.hh"
#include "CalcWPPixel.hh"

namespace comptonsoft {

SimDetectorUnit2DPad::SimDetectorUnit2DPad()
  : m_UpsideReadElectrode(false),
    m_UseSymmetry(true),
    m_CCEMap(0),
    m_WPMap(0)
{
  registerDetectorUnit( static_cast<RealDetectorUnit*>(this) );
}


SimDetectorUnit2DPad::~SimDetectorUnit2DPad()
{
}


void SimDetectorUnit2DPad::setThresholdEnergyCathode(double val)
{
  if (UpsideReadElectrode()==UpsideCathode()) {
    setThresholdEnergy(val);
  }
}


void SimDetectorUnit2DPad::setThresholdEnergyAnode(double val)
{
  if (UpsideReadElectrode()==UpsideAnode()) {
    setThresholdEnergy(val);
  }
}


double SimDetectorUnit2DPad::ThresholdEnergyCathode()
{
  if (UpsideReadElectrode() != UpsideCathode()) {
    return 0.0;
  }
  return ThresholdEnergyX();
}


double SimDetectorUnit2DPad::ThresholdEnergyAnode()
{
  if (UpsideReadElectrode() != UpsideAnode()) {
    return 0.0;
  }
  return ThresholdEnergyX();
}


void SimDetectorUnit2DPad::simulatePulseHeights()
{
  std::vector<DetectorHit_sptr>::iterator it;
  std::vector<DetectorHit_sptr>::iterator itEnd = m_RawHits.end();
  
  for (it = m_RawHits.begin(); it != itEnd; it++) {
    DetectorHit_sptr rawhit = *it;
    double edep = rawhit->getEdep();
    double localposx = rawhit->getLocalPosX();
    double localposy = rawhit->getLocalPosY();
    double localposz = rawhit->getLocalPosZ();

    if (edep == 0.0) {
      rawhit->addFlag(PRIORITY_SIDE);
      if (UpsideReadElectrode()==UpsideAnode()) {
        rawhit->addFlag(ANODE_SIDE);
      }
      fillStrip(rawhit);
      insertSimulatedHit(rawhit);
      continue;
    }

    StripPair sp = StripXYByLocalPosition(localposx, localposy);
    
    if (SimDiffusionMode() == 0) {
      DetectorHit_sptr hit = generateHit(*rawhit, sp);
      insertSimulatedHit(hit);
    }
    else {
      DetectorHit_sptr hit = generateHit(*rawhit, sp);
      double pha = hit->getPHA();

      int numDivision = DiffusionDivisionNumber();
      double edepDivision = edep/numDivision;
      double phaDivision = pha/numDivision;
      
      double diffusionSigma = 0.0;
      if (UpsideReadElectrode() == UpsideAnode()) {
        // anode readout
        diffusionSigma = DiffusionSigmaAnode(localposz);
      }
      else {
        // cathode readout
        diffusionSigma = DiffusionSigmaCathode(localposz);
      }

      std::vector<DetectorHit_sptr> diffusionHits;
      std::vector<DetectorHit_sptr>::iterator itHit;
      for (int l=0; l<numDivision; l++) {
        double radiusDiffusion = gRandom->Gaus(0.0, diffusionSigma);
        double thetaDiffusion = gRandom->Uniform(TMath::TwoPi());
        double dx = radiusDiffusion * TMath::Cos(thetaDiffusion);
        double dy = radiusDiffusion * TMath::Sin(thetaDiffusion);
        StripPair spDiff = StripXYByLocalPosition(localposx+dx, localposy+dy);

        using namespace boost::lambda;
        itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                        bind(&DetectorHit::getStrip, *_1) == spDiff);
        
        if (itHit != diffusionHits.end()) {
          (*itHit)->setEdep( (*itHit)->getEdep() + edepDivision );
          (*itHit)->setPHA( (*itHit)->getPHA() + phaDivision );
        }
        else {
          DetectorHit_sptr hitDivision(new DetectorHit(*hit));
          hitDivision->setEdep(edepDivision);
          hitDivision->setPHA(phaDivision);
          hitDivision->setStrip(spDiff);
          diffusionHits.push_back(hitDivision);
        }
      }

      for (itHit=diffusionHits.begin(); itHit!=diffusionHits.end(); itHit++) {
        insertSimulatedHit(*itHit);
      }
    }

    // adjacent pad
    if (SimPHAMode()==3) {
      DetectorHit_sptr hit;
      StripPair adjacentPixel;
     
      adjacentPixel.set(sp.X()+1, sp.Y()+1);
      hit = generateHit(*rawhit, adjacentPixel);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(sp.X()+1, sp.Y());
      hit = generateHit(*rawhit, adjacentPixel);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(sp.X()+1, sp.Y()-1);
      hit = generateHit(*rawhit, adjacentPixel);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(sp.X()  , sp.Y()+1);
      hit = generateHit(*rawhit, adjacentPixel);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(sp.X()  , sp.Y()-1);
      hit = generateHit(*rawhit, adjacentPixel);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(sp.X()-1, sp.Y()+1);
      hit = generateHit(*rawhit, adjacentPixel);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(sp.X()-1, sp.Y()  );
      hit = generateHit(*rawhit, adjacentPixel);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(sp.X()-1, sp.Y()-1);
      hit = generateHit(*rawhit, adjacentPixel);
      insertSimulatedHit(hit);
    }
  }
}


DetectorHit_sptr SimDetectorUnit2DPad::generateHit(const DetectorHit& rawhit,
                                                   const StripPair& sp)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  hit->addFlag(PRIORITY_SIDE);
  if (UpsideReadElectrode()==UpsideAnode()) {
    hit->addFlag(ANODE_SIDE);
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


double SimDetectorUnit2DPad::ChargeCollectionEfficiency(const StripPair& sp,
                                                        double x,
                                                        double y,
                                                        double z)
{
  double xPixel, yPixel, zPixel;
  LocalPosition(sp, &xPixel, &yPixel, &zPixel);
  
  double xInPixel = x - xPixel;
  double yInPixel = y - yPixel;
  
  int ix = m_CCEMap->GetXaxis()->FindBin(xInPixel);
  int iy = m_CCEMap->GetYaxis()->FindBin(yInPixel);
  int iz = m_CCEMap->GetZaxis()->FindBin(z);

  return m_CCEMap->GetBinContent(ix, iy, iz);
}


double SimDetectorUnit2DPad::WeightingPotential(const StripPair& sp,
                                                double x, double y, double z)
{
  double xPixel, yPixel, zPixel;
  Position(sp, &xPixel, &yPixel, &zPixel);
  
  double xInPixel = x - xPixel;
  double yInPixel = y - yPixel;
  
  int ix = m_WPMap->GetXaxis()->FindBin(xInPixel);
  int iy = m_WPMap->GetYaxis()->FindBin(yInPixel);
  int iz = m_WPMap->GetZaxis()->FindBin(z);

  return m_WPMap->GetBinContent(ix, iy, iz);
}


bool SimDetectorUnit2DPad::isCCEMapPrepared()
{
  return (getCCEMap()!=0);
}


void SimDetectorUnit2DPad::buildWPMap()
{
  if (SimPHAMode() == 2) {
    buildWPMap(31, 31, 64, 1.0);
  }
  else if (SimPHAMode() == 3) {
    buildWPMap(93, 93, 64, 3.0);
  }
  else {
    std::cout << "Error : buildWPMap() " << std::endl;
    return;
  }
}


void SimDetectorUnit2DPad::buildWPMap(int nx, int ny, int nz,
                                      double pixel_factor)
{
  if (nx<1 || ny<1 || nz<=1) {
    std::cout << "Error : buildWPMap " << std::endl;
    return;
  }

  const double MapSizeX = getPixelPitchX()*pixel_factor;
  const double MapSizeY = getPixelPitchY()*pixel_factor;
  const double MapSizeZ
    = getThickness()*static_cast<double>(nz)/static_cast<double>(nz-1);

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
  sprintf(histname, "wp_%04d", getID());
  m_WPMap =  new TH3D(histname, histname,
                      nx, -0.5*MapSizeX, +0.5*MapSizeX,
                      ny, -0.5*MapSizeY, +0.5*MapSizeY,
                      nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
 
  double x, y, z;
  boost::scoped_ptr<CalcWPPixel> calc(new CalcWPPixel);
  calc->set_geometry(getPixelPitchX(), getPixelPitchY(), getThickness(),
                           NumPixelInWPCalculation());
  calc->init_table();
  std::cout << "calculating weighing potential..." << std::endl;
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    x = m_WPMap->GetXaxis()->GetBinCenter(ix);
    for (int iy=1; iy<=ny; iy++) {
      y = m_WPMap->GetYaxis()->GetBinCenter(iy);
      calc->set_xy(x, y);
      for (int iz=1; iz<=nz; iz++) {
        z = m_WPMap->GetZaxis()->GetBinCenter(iz);
        if (UpsideReadElectrode()) {
          double wp = 0.;
          if (iz==nz) {
            if (TMath::Abs(x)<0.5*getPixelPitchX() && TMath::Abs(y)<0.5*getPixelPitchY()) {
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
          m_WPMap->SetBinContent(ix, iy, iz, wp);
        }
        else {
          double wp = 0.;
          if (iz==1) {
            if (TMath::Abs(x)<0.5*getPixelPitchX() && TMath::Abs(y)<0.5*getPixelPitchY()) {
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
          m_WPMap->SetBinContent(ix, iy, iz, wp);
        }
      }
    }
  }
  std::cout << std::endl;
}


void SimDetectorUnit2DPad::buildCCEMap()
{
  if (SimPHAMode() == 2) {
    buildCCEMap(31, 31, 63, 1.0);
  }
  else if (SimPHAMode() == 3) {
    buildCCEMap(93, 93, 63, 3.0);
  }
  else {
    std::cout << "Error : buildCCEMap() " << std::endl;
    return;
  }
}


void SimDetectorUnit2DPad::buildCCEMap(int nx, int ny, int nz,
                                       double pixel_factor)
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
  sprintf(histname, "cce_%04d", getID());
  m_CCEMap =  new TH3D(histname, histname,
                       nx, -0.5*MapSizeX, +0.5*MapSizeX,
                       ny, -0.5*MapSizeY, +0.5*MapSizeY,
                       nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
  
  std::cout << "calculating charge collection efficiency..." << std::endl;
  
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    for (int iy=1; iy<=ny; iy++) {
      if (!m_UseSymmetry || (ix<=(nx+1)/2 && iy<=(ny+1)/2)) {
        double x = m_CCEMap->GetXaxis()->GetBinCenter(ix);
        double y = m_CCEMap->GetYaxis()->GetBinCenter(iy);
        int binx = m_WPMap->GetXaxis()->FindBin(x);
        int biny = m_WPMap->GetYaxis()->FindBin(y);

        int numPoints = nz+1;
        boost::shared_array<double> wp(new double[numPoints]);
        for (int k=0; k<numPoints; k++) {
          double z = m_CCEMap->GetZaxis()->GetBinLowEdge(k+1);
          int binz = m_WPMap->GetZaxis()->FindBin(z);
          wp[k] = m_WPMap->GetBinContent(binx, biny, binz);
        }
        setWPForCCECalculation(UpsideReadElectrode(), wp, numPoints);

        for (int iz=1; iz<=nz; iz++) {
          double z = m_CCEMap->GetZaxis()->GetBinCenter(iz);
          double cce = calculateCCE(z);
          m_CCEMap->SetBinContent(ix, iy, iz, cce);
        }
      }
    }
  }

  if (m_UseSymmetry) {
    for (int ix=1; ix<=nx; ix++) {
      for (int iy=1; iy<=ny; iy++) {
        for (int iz=1; iz<=nz; iz++) {
          if (ix>(nx+1)/2 && iy<=(ny+1)/2) {
            double cce = m_CCEMap->GetBinContent(nx-ix+1, iy, iz);
            m_CCEMap->SetBinContent(ix, iy, iz, cce);
          }
          else if (ix<=(nx+1)/2 && iy>(ny+1)/2) {
            double cce = m_CCEMap->GetBinContent(ix, ny-iy+1, iz);
            m_CCEMap->SetBinContent(ix, iy, iz, cce);
          }
          else if (ix>(ny+1)/2 && iy>(ny+1)/2) {
            double cce = m_CCEMap->GetBinContent(nx-ix+1, ny-iy+1, iz);
            m_CCEMap->SetBinContent(ix, iy, iz, cce);
          }
        }
      }
    }
  }

  std::cout << std::endl;
  std::cout << "Charge collection efficiency map is built." << std::endl;
}


void SimDetectorUnit2DPad::printSimParam()
{
  std::cout << "SimDetectorUnit2DPad:" << '\n'
            << " upside pad : " << UpsideReadElectrode() << '\n'
            << std::endl;
  DeviceSimulation::printSimParam();
}
  
}
