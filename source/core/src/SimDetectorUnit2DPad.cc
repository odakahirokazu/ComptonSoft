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

#include "SimDetectorUnit2DPad.hh"

#include <iostream>
#include <algorithm>
#include <memory>

#include "TMath.h"
#include "TRandom3.h"
#include "TH3.h"

#include "FlagDefinition.hh"
#include "DetectorHit.hh"
#include "CalcWPPixel.hh"

namespace comptonsoft {

SimDetectorUnit2DPad::SimDetectorUnit2DPad()
  : usingSymmetry_(true),
    CCEMap_(nullptr),
    WPMap_(nullptr)
{
}

SimDetectorUnit2DPad::~SimDetectorUnit2DPad() = default;

void SimDetectorUnit2DPad::initializeEvent()
{
  RealDetectorUnit2DPad::initializeEvent();
  DeviceSimulation::initializeEvent();
}

void SimDetectorUnit2DPad::simulatePulseHeights()
{
  const int N = NumberOfRawHits();
  for (int i=0; i<N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    double edep = rawhit->EnergyDeposit();
    double localposx = rawhit->LocalPositionX();
    double localposy = rawhit->LocalPositionY();
    double localposz = rawhit->LocalPositionZ();

    if (edep == 0.0) {
      rawhit->addFlags(flag::PrioritySide);
      if (isAnodeReadout()) {
        rawhit->addFlags(flag::AnodeSide);
      }
      else if (isCathodeReadout()) {
        rawhit->addFlags(flag::CathodeSide);
      }
      
      fillPixel(rawhit);
      insertSimulatedHit(rawhit);
      continue;
    }

    PixelID pixel = findPixel(localposx, localposy);
    
    if (SimDiffusionMode() == 0) {
      DetectorHit_sptr hit = generateHit(*rawhit, pixel);
      insertSimulatedHit(hit);
    }
    else {
      DetectorHit_sptr hit = generateHit(*rawhit, pixel);
      double energyCharge = hit->EnergyCharge();

      const int numDivision = DiffusionDivisionNumber();
      const double edepDivision = edep/numDivision;
      const double energyChargeDivision = energyCharge/numDivision;
      
      double diffusionSigma = 0.0;
      if (isAnodeReadout()) {
        diffusionSigma = DiffusionSigmaAnode(localposz);
      }
      else if (isCathodeReadout()) {
        diffusionSigma = DiffusionSigmaCathode(localposz);
      }

      std::vector<DetectorHit_sptr> diffusionHits;
      for (int l=0; l<numDivision; l++) {
        double radiusDiffusion = gRandom->Gaus(0.0, diffusionSigma);
        double thetaDiffusion = gRandom->Uniform(TMath::TwoPi());
        double dx = radiusDiffusion * TMath::Cos(thetaDiffusion);
        double dy = radiusDiffusion * TMath::Sin(thetaDiffusion);
        PixelID pixelDiff = findPixel(localposx+dx, localposy+dy);

        auto itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                             [&](const DetectorHit_sptr& hit) {
                               return (hit->Pixel()==pixelDiff);
                             });
        if (itHit != diffusionHits.end()) {
          DetectorHit_sptr& hit = *itHit;
          hit->setEnergyDeposit( hit->EnergyDeposit() + edepDivision );
          hit->setEnergyCharge( hit->EnergyCharge() + energyChargeDivision );
        }
        else {
          DetectorHit_sptr hitDivision(new DetectorHit(*hit));
          hitDivision->setEnergyDeposit(edepDivision);
          hitDivision->setEnergyCharge(energyChargeDivision);
          hitDivision->setPixel(pixelDiff);
          diffusionHits.push_back(hitDivision);
        }
      }

      for (auto itHit=diffusionHits.begin(); itHit!=diffusionHits.end(); itHit++) {
        DetectorHit_sptr& hit = *itHit;
        insertSimulatedHit(hit);
      }
    }

    // adjacent pad
    if (SimPHAMode()==3) {
      DetectorHit_sptr hit;
      PixelID adjacentPixel;
     
      adjacentPixel.set(pixel.X()+1, pixel.Y()+1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(pixel.X()+1, pixel.Y());
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(pixel.X()+1, pixel.Y()-1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(pixel.X()  , pixel.Y()+1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(pixel.X()  , pixel.Y()-1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(pixel.X()-1, pixel.Y()+1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(pixel.X()-1, pixel.Y()  );
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
      
      adjacentPixel.set(pixel.X()-1, pixel.Y()-1);
      hit = generateHit(*rawhit, adjacentPixel);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
    }
  }
}

DetectorHit_sptr SimDetectorUnit2DPad::generateHit(const DetectorHit& rawhit,
                                                   const PixelID& pixel)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  hit->addFlags(flag::PrioritySide);
  if (isAnodeReadout()) {
    hit->addFlags(flag::AnodeSide);
  }
  else if (isCathodeReadout()) {
    hit->addFlags(flag::CathodeSide);
  }
    
  if (checkRange(pixel)) {
    hit->setPixel(pixel);
  }
  else {
    hit->setPixel(PixelID::OnGuardRing, PixelID::OnGuardRing);
    return hit;
  }

  double edep = hit->EnergyDeposit();
  double localposx = hit->LocalPositionX();
  double localposy = hit->LocalPositionY();
  double localposz = hit->LocalPositionZ();

  if (hit->isProcess(process::NucleusHit)) {
    edep *= QuenchingFactor();
  }

  double energyCharge = calculateEnergyCharge(pixel, edep,
                                              localposx, localposy, localposz);
  hit->setEnergyCharge(energyCharge);

  return hit;
}

double SimDetectorUnit2DPad::
ChargeCollectionEfficiency(const PixelID& pixel,
                           double x,
                           double y,
                           double z) const
{
  double xPixel, yPixel, zPixel;
  LocalPosition(pixel, &xPixel, &yPixel, &zPixel);
  
  double xInPixel = x - xPixel;
  double yInPixel = y - yPixel;
  
  int ix = CCEMap_->GetXaxis()->FindBin(xInPixel);
  int iy = CCEMap_->GetYaxis()->FindBin(yInPixel);
  int iz = CCEMap_->GetZaxis()->FindBin(z);

  return CCEMap_->GetBinContent(ix, iy, iz);
}

double SimDetectorUnit2DPad::WeightingPotential(const PixelID& pixel,
                                                double x, double y, double z)
{
  double xPixel, yPixel, zPixel;
  Position(pixel, &xPixel, &yPixel, &zPixel);
  
  double xInPixel = x - xPixel;
  double yInPixel = y - yPixel;
  
  int ix = WPMap_->GetXaxis()->FindBin(xInPixel);
  int iy = WPMap_->GetYaxis()->FindBin(yInPixel);
  int iz = WPMap_->GetZaxis()->FindBin(z);

  return WPMap_->GetBinContent(ix, iy, iz);
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
  WPMap_ =  new TH3D(histname, histname,
                      nx, -0.5*MapSizeX, +0.5*MapSizeX,
                      ny, -0.5*MapSizeY, +0.5*MapSizeY,
                      nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
 
  double x, y, z;
  std::unique_ptr<CalcWPPixel> calc(new CalcWPPixel);
  calc->setGeometry(getPixelPitchX(), getPixelPitchY(), getThickness(),
                    NumPixelsInWPCalculation());
  calc->initializeTable();
  std::cout << "calculating weighing potential..." << std::endl;
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    x = WPMap_->GetXaxis()->GetBinCenter(ix);
    for (int iy=1; iy<=ny; iy++) {
      y = WPMap_->GetYaxis()->GetBinCenter(iy);
      calc->setXY(x, y);
      for (int iz=1; iz<=nz; iz++) {
        z = WPMap_->GetZaxis()->GetBinCenter(iz);
        if (isUpSideReadout()) {
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
            wp = calc->WeightingPotential(z);
          }
          WPMap_->SetBinContent(ix, iy, iz, wp);
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
            wp = calc->WeightingPotential(-z);
          }
          WPMap_->SetBinContent(ix, iy, iz, wp);
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
  CCEMap_ =  new TH3D(histname, histname,
                       nx, -0.5*MapSizeX, +0.5*MapSizeX,
                       ny, -0.5*MapSizeY, +0.5*MapSizeY,
                       nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
  
  std::cout << "calculating charge collection efficiency..." << std::endl;
  
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    for (int iy=1; iy<=ny; iy++) {
      if (!isUsingSymmetry() || (ix<=(nx+1)/2 && iy<=(ny+1)/2)) {
        double x = CCEMap_->GetXaxis()->GetBinCenter(ix);
        double y = CCEMap_->GetYaxis()->GetBinCenter(iy);
        int binx = WPMap_->GetXaxis()->FindBin(x);
        int biny = WPMap_->GetYaxis()->FindBin(y);

        int numPoints = nz+1;
        boost::shared_array<double> wp(new double[numPoints]);
        for (int k=0; k<numPoints; k++) {
          double z = CCEMap_->GetZaxis()->GetBinLowEdge(k+1);
          int binz = WPMap_->GetZaxis()->FindBin(z);
          wp[k] = WPMap_->GetBinContent(binx, biny, binz);
        }
        setWeightingPotential(isUpSideReadout(), wp, numPoints);

        for (int iz=1; iz<=nz; iz++) {
          double z = CCEMap_->GetZaxis()->GetBinCenter(iz);
          double cce = calculateCCE(z);
          CCEMap_->SetBinContent(ix, iy, iz, cce);
        }
      }
    }
  }

  if (isUsingSymmetry()) {
    for (int ix=1; ix<=nx; ix++) {
      for (int iy=1; iy<=ny; iy++) {
        for (int iz=1; iz<=nz; iz++) {
          if (ix>(nx+1)/2 && iy<=(ny+1)/2) {
            double cce = CCEMap_->GetBinContent(nx-ix+1, iy, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
          else if (ix<=(nx+1)/2 && iy>(ny+1)/2) {
            double cce = CCEMap_->GetBinContent(ix, ny-iy+1, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
          else if (ix>(ny+1)/2 && iy>(ny+1)/2) {
            double cce = CCEMap_->GetBinContent(nx-ix+1, ny-iy+1, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
        }
      }
    }
  }

  std::cout << std::endl;
  std::cout << "Charge collection efficiency map is built." << std::endl;
}

void SimDetectorUnit2DPad::printSimulationParameters()
{
  std::cout << "SimDetectorUnit2DPad:" << '\n'
            << " upside pad : " << isUpSideReadout() << '\n'
            << std::endl;
  DeviceSimulation::printSimulationParameters();
}
  
} /* namespace comptonsoft */
