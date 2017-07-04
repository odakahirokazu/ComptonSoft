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

#include "SimDetectorUnit2DStrip.hh"

#include <iostream>
#include <memory>
#include <boost/format.hpp>
#include "TMath.h"
#include "TRandom3.h"

#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"
#include "CalcWPStrip.hh"

namespace comptonsoft {

SimDetectorUnit2DStrip::SimDetectorUnit2DStrip()
  : usingSymmetry_(true),
    CCEMapXStrip_(0),
    CCEMapYStrip_(0),
    WPMapXStrip_(0),
    WPMapYStrip_(0)
{
  setNumPixelsInWPCalculation(20.0);
}

SimDetectorUnit2DStrip::~SimDetectorUnit2DStrip() = default;

void SimDetectorUnit2DStrip::initializeEvent()
{
  RealDetectorUnit2DStrip::initializeEvent();
  DeviceSimulation::initializeEvent();
}

void SimDetectorUnit2DStrip::simulatePulseHeights()
{
  const int N = NumberOfRawHits();
  for (int i=0; i<N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    double edep = rawhit->EnergyDeposit();
    double localposx = rawhit->LocalPositionX();
    double localposy = rawhit->LocalPositionY();
    double localposz = rawhit->LocalPositionZ();
    PixelID sp = findPixel(localposx, localposy);
    
    if (edep == 0.0) {
      DetectorHit_sptr xhit(new DetectorHit(*rawhit));
      xhit->setPixel(sp.X(), PixelID::Undefined);
      if (isXStripSideAnode()) {
        xhit->addFlags(flag::AnodeSide);
        xhit->addFlags(PriorityToAnodeSide() ? flag::PrioritySide : 0);
      }
      else if (isXStripSideCathode()) {
        xhit->addFlags(flag::CathodeSide);
        xhit->addFlags(PriorityToCathodeSide() ? flag::PrioritySide : 0);
      }
      
      DetectorHit_sptr yhit(new DetectorHit(*rawhit));
      yhit->setPixel(PixelID::Undefined, sp.Y());
      if (isYStripSideAnode()) {
        yhit->addFlags(flag::AnodeSide);
        yhit->addFlags(PriorityToAnodeSide() ? flag::PrioritySide : 0);
      }
      else if (isYStripSideCathode()) {
        yhit->addFlags(flag::CathodeSide);
        yhit->addFlags(PriorityToCathodeSide() ? flag::PrioritySide : 0);
      }
  
      insertSimulatedHit(xhit);
      insertSimulatedHit(yhit);
      continue;
    }

    if (DiffusionMode()==0) {
      PixelID xsp(sp.X(), PixelID::Undefined);
      PixelID ysp(PixelID::Undefined, sp.Y());
      DetectorHit_sptr xhit = generateHit(*rawhit, xsp);
      DetectorHit_sptr yhit = generateHit(*rawhit, ysp);
      insertSimulatedHit(xhit);
      insertSimulatedHit(yhit);
    }
    else {
      PixelID xsp(sp.X(), PixelID::Undefined);
      PixelID ysp(PixelID::Undefined, sp.Y());
      DetectorHit_sptr xhit = generateHit(*rawhit, xsp);
      DetectorHit_sptr yhit = generateHit(*rawhit, ysp);
      double xECharge = xhit->EnergyCharge();
      double yECharge = yhit->EnergyCharge();

      int numDivision = DiffusionDivisionNumber();
      double edepDivision = edep/numDivision;
      double xEChargeDivision = xECharge/numDivision;
      double yEChargeDivision = yECharge/numDivision;
      
      double xDiffusionSigma = 0.0;
      double yDiffusionSigma = 0.0;
      if (isXStripSideAnode()) {
        xDiffusionSigma = DiffusionSigmaAnode(localposz);
        yDiffusionSigma = DiffusionSigmaCathode(localposz);
      }
      else if (isXStripSideCathode()) {
        xDiffusionSigma = DiffusionSigmaCathode(localposz);
        yDiffusionSigma = DiffusionSigmaAnode(localposz);
      }

      { // x-strip
        std::vector<DetectorHit_sptr> diffusionHits;
        for (int l=0; l<numDivision; l++) {
          double radiusDiffusion = gRandom->Gaus(0.0, xDiffusionSigma);
          double thetaDiffusion = gRandom->Uniform(CLHEP::twopi);
          double dx = radiusDiffusion * std::cos(thetaDiffusion);
          double dy = radiusDiffusion * std::sin(thetaDiffusion);
          PixelID spDiff = findPixel(localposx+dx, localposy+dy);
          spDiff.setY(PixelID::Undefined);
          
          auto itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                               [&](const DetectorHit_sptr& hit)-> bool {
                                 return (hit->Pixel()==spDiff);
                               });
          if (itHit != diffusionHits.end()) {
            DetectorHit_sptr hit = *itHit;
            hit->setEnergyDeposit( hit->EnergyDeposit() + edepDivision );
            hit->setEnergyCharge( hit->EnergyCharge() + xEChargeDivision );
          }
          else {
            DetectorHit_sptr hitDivision(new DetectorHit(*xhit));
            hitDivision->setEnergyDeposit(edepDivision);
            hitDivision->setEnergyCharge(xEChargeDivision);
            hitDivision->setPixel(spDiff);
            diffusionHits.push_back(hitDivision);
          }
        }
        
        for (auto& hit: diffusionHits) {
          insertSimulatedHit(hit);
        }
      }

      { // y-strip
        std::vector<DetectorHit_sptr> diffusionHits;
        for (int l=0; l<numDivision; l++) {
          double radiusDiffusion = gRandom->Gaus(0.0, yDiffusionSigma);
          double thetaDiffusion = gRandom->Uniform(CLHEP::twopi);
          double dx = radiusDiffusion * std::cos(thetaDiffusion);
          double dy = radiusDiffusion * std::sin(thetaDiffusion);
          PixelID spDiff = findPixel(localposx+dx, localposy+dy);
          spDiff.setX(PixelID::Undefined);
          
          auto itHit = find_if(diffusionHits.begin(), diffusionHits.end(),
                               [&](const DetectorHit_sptr& hit)-> bool {
                                 return (hit->Pixel()==spDiff);
                               });
          if (itHit != diffusionHits.end()) {
            DetectorHit_sptr hit = *itHit;
            (*itHit)->setEnergyDeposit( hit->EnergyDeposit() + edepDivision );
            (*itHit)->setEnergyCharge( hit->EnergyCharge() + yEChargeDivision );
          }
          else {
            DetectorHit_sptr hitDivision(new DetectorHit(*yhit));
            hitDivision->setEnergyDeposit(edepDivision);
            hitDivision->setEnergyCharge(yEChargeDivision);
            hitDivision->setPixel(spDiff);
            diffusionHits.push_back(hitDivision);
          }
        }

        for (auto& hit: diffusionHits) {
          insertSimulatedHit(hit);
        }
      }
    }
      
    // near strips
    if (ChargeCollectionMode()==3) {
      DetectorHit_sptr hit;
      PixelID nearStrip;
     
      nearStrip.set(sp.X()+2, PixelID::Undefined);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()+1, PixelID::Undefined);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()-1, PixelID::Undefined);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(sp.X()-2, PixelID::Undefined);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);
    
      nearStrip.set(PixelID::Undefined, sp.Y()+2);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(PixelID::Undefined, sp.Y()+1);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(PixelID::Undefined, sp.Y()-1);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);

      nearStrip.set(PixelID::Undefined, sp.Y()-2);
      hit = generateHit(*rawhit, nearStrip);
      hit->setEnergyDeposit(0.0);
      insertSimulatedHit(hit);   
    }
  }
}

DetectorHit_sptr SimDetectorUnit2DStrip::generateHit(const DetectorHit& rawhit,
                                                     const PixelID& sp)
{
  DetectorHit_sptr hit(new DetectorHit(rawhit));
  if (sp.isXStrip()) {
    if (isXStripSideAnode()) {
      hit->addFlags(flag::AnodeSide);
      hit->addFlags(PriorityToAnodeSide() ? flag::PrioritySide : 0);
    }
    else if (isXStripSideCathode()) {
      hit->addFlags(flag::CathodeSide);
      hit->addFlags(PriorityToCathodeSide() ? flag::PrioritySide : 0);
    }
  }
  else if (sp.isYStrip()) {
    if (isYStripSideAnode()) {
      hit->addFlags(flag::AnodeSide);
      hit->addFlags(PriorityToAnodeSide() ? flag::PrioritySide : 0);
    }
    else if (isYStripSideCathode()) {
      hit->addFlags(flag::CathodeSide);
      hit->addFlags(PriorityToCathodeSide() ? flag::PrioritySide : 0);
    }
  }
  
  if (checkRange(sp)) {
    hit->setPixel(sp);
  }
  else {
    hit->setPixel(PixelID::OnGuardRing, PixelID::OnGuardRing);
    return hit;
  }

  applyQuenching(hit);

  const double edep = hit->EnergyDeposit();
  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const double localposz = hit->LocalPositionZ();
  const double energyCharge = calculateEnergyCharge(sp,
                                                    edep,
                                                    localposx,
                                                    localposy,
                                                    localposz);
  hit->setEnergyCharge(energyCharge);
  
  return hit;
}

double SimDetectorUnit2DStrip::
ChargeCollectionEfficiency(const PixelID& sp,
                           double x, double y, double z) const
{
  double xPixel, yPixel, zPixel;
  LocalPosition(sp, &xPixel, &yPixel, &zPixel);
  
  double cce = 1.0;
  if (sp.isXStrip()) {
    double xInPixel = x - xPixel;
    int ix = CCEMapXStrip_->GetXaxis()->FindBin(xInPixel);
    int iz = CCEMapXStrip_->GetYaxis()->FindBin(z);
    cce =  CCEMapXStrip_->GetBinContent(ix, iz);
  }
  else if (sp.isYStrip()) {
    double yInPixel = y - yPixel;
    int iy = CCEMapYStrip_->GetXaxis()->FindBin(yInPixel);
    int iz = CCEMapYStrip_->GetYaxis()->FindBin(z);
    cce =  CCEMapYStrip_->GetBinContent(iy, iz);
  }

  return cce;
}

double SimDetectorUnit2DStrip::WeightingPotential(const PixelID& sp, double x, double y, double z)
{
  double xPixel, yPixel, zPixel;
  Position(sp, &xPixel, &yPixel, &zPixel);
  
  double xInPixel = x - xPixel;
  double yInPixel = y - yPixel;

  double wp = 0.0;
  if (sp.isXStrip()) {
    int ix = WPMapXStrip_->GetXaxis()->FindBin(xInPixel);
    int iz = WPMapXStrip_->GetYaxis()->FindBin(z);
    wp =  WPMapXStrip_->GetBinContent(ix, iz);
  }
  else if (sp.isYStrip()) {
    int iy = WPMapYStrip_->GetXaxis()->FindBin(yInPixel);
    int iz = WPMapYStrip_->GetYaxis()->FindBin(z);
    wp =  WPMapYStrip_->GetBinContent(iy, iz);
  }

  return wp;
}

bool SimDetectorUnit2DStrip::isCCEMapPrepared()
{
  return (getCCEMapXStrip()!=0 && getCCEMapYStrip()!=0);
}

void SimDetectorUnit2DStrip::buildWPMap()
{
  if (ChargeCollectionMode() == 2) {
    buildWPMap(19, 19, 128, 1.0);
  }
  else if (ChargeCollectionMode() == 3) {
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

  if (ChargeCollectionMode() == 2) {
    if (MapSizeX<getPixelPitchX()*0.999 || MapSizeY<getPixelPitchY()*0.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 2.";
    }
  }
  else if (ChargeCollectionMode() == 3) {
    if (MapSizeX<getPixelPitchX()*4.999 || MapSizeY<getPixelPitchY()*4.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 3.";
    }
  }
  
  std::string histname = (boost::format("wp_x_%04d")%getID()).str();
  WPMapXStrip_ =  new TH2D(histname.c_str(), histname.c_str(),
                            nx, -0.5*MapSizeX, +0.5*MapSizeX,
                            nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
  histname = (boost::format("wp_y_%04d")%getID()).str();
  WPMapYStrip_ =  new TH2D(histname.c_str(), histname.c_str(),
                            ny, -0.5*MapSizeY, +0.5*MapSizeY,
                            nz, -0.5*MapSizeZ, +0.5*MapSizeZ);

  std::cout << "calculating weighing potential..." << std::endl;

  double x, y, z;
  std::unique_ptr<CalcWPStrip> calc(new CalcWPStrip);
  calc->setGeometry(getPixelPitchX(), getThickness(),
                    NumPixelsInWPCalculation());
  calc->initializeTable();
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    x = WPMapXStrip_->GetXaxis()->GetBinCenter(ix);
    calc->setX(x);
    for (int iz=1; iz<=nz; iz++) {
      z = WPMapXStrip_->GetYaxis()->GetBinCenter(iz);
      if (isUpSideXStrip()) {
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
          wp = calc->WeightingPotential(z);
        }
        WPMapXStrip_->SetBinContent(ix, iz, wp);
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
          wp = calc->WeightingPotential(-z);
        }
        WPMapXStrip_->SetBinContent(ix, iz, wp);
      }
    }
  }

  calc->setGeometry(getPixelPitchY(), getThickness(),
                    NumPixelsInWPCalculation());
  calc->initializeTable();
  for (int iy=1; iy<=ny; iy++) {
    std::cout << '*' << std::flush;
    y = WPMapYStrip_->GetXaxis()->GetBinCenter(iy);
    calc->setX(y);
    for (int iz=1; iz<=nz; iz++) {
      z = WPMapYStrip_->GetYaxis()->GetBinCenter(iz);
      if (isUpSideYStrip()) {
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
          wp = calc->WeightingPotential(z);
        }
        WPMapYStrip_->SetBinContent(iy, iz, wp);
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
          wp = calc->WeightingPotential(-z);
        }
        WPMapYStrip_->SetBinContent(iy, iz, wp);
      }
    }
  }
  
  std::cout << std::endl;
}

void SimDetectorUnit2DStrip::buildCCEMap()
{
  if (ChargeCollectionMode() == 2) {
    buildCCEMap(19, 19, 127, 1.0);
  }
  else if (ChargeCollectionMode() == 3) {
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

  if (ChargeCollectionMode() == 2) {
    if (MapSizeX<getPixelPitchX()*0.999 || MapSizeY<getPixelPitchY()*0.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 2.";
    }
  }
  else if (ChargeCollectionMode() == 3) {
    if (MapSizeX<getPixelPitchX()*2.999 || MapSizeY<getPixelPitchY()*2.999) {
      std::cout << "Warning: map size is smaller than required in sim mode 3.";
    }
  }
  
  std::string histname = (boost::format("cce_x_%04d")%getID()).str();
  CCEMapXStrip_ =  new TH2D(histname.c_str(), histname.c_str(),
                             nx, -0.5*MapSizeX, +0.5*MapSizeX,
                             nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
  histname = (boost::format("cce_y_%04d")%getID()).str();
  CCEMapYStrip_ =  new TH2D(histname.c_str(), histname.c_str(),
                             nx, -0.5*MapSizeY, +0.5*MapSizeY,
                             nz, -0.5*MapSizeZ, +0.5*MapSizeZ);
  
  std::cout << "calculating charge collection efficiency..." << std::endl;
  
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    if (!isUsingSymmetry() || ix<=(nx+1)/2) {
      double x = CCEMapXStrip_->GetXaxis()->GetBinCenter(ix);
      int binx = WPMapXStrip_->GetXaxis()->FindBin(x);
      
      int numPoints = nz+1;
      boost::shared_array<double> wp(new double[numPoints]);
      for (int k=0; k<numPoints; k++) {
        double z = CCEMapXStrip_->GetYaxis()->GetBinLowEdge(k+1);
        int binz = WPMapXStrip_->GetYaxis()->FindBin(z);
        wp[k] = WPMapXStrip_->GetBinContent(binx, binz);
      }
      setWeightingPotential(isUpSideXStrip(), wp, numPoints);
      
      for (int iz=1; iz<=nz; iz++) {
        double z = CCEMapXStrip_->GetYaxis()->GetBinCenter(iz);
        double cce = calculateCCE(z);
        // std::cout << cce << std::endl;
        CCEMapXStrip_->SetBinContent(ix, iz, cce);
      }
    }
  }

  if (isUsingSymmetry()) {
    for (int ix=1; ix<=nx; ix++) {
      for (int iz=1; iz<=nz; iz++) {
        if (ix>(nx+1)/2) {
          double cce = CCEMapXStrip_->GetBinContent(nx-ix+1, iz);
          CCEMapXStrip_->SetBinContent(ix, iz, cce);
        }
      }
    }
  }

  for (int iy=1; iy<=ny; iy++) {
    std::cout << '*' << std::flush;
    if (!isUsingSymmetry() || iy<=(ny+1)/2) {
      double y = CCEMapYStrip_->GetXaxis()->GetBinCenter(iy);
      int biny = WPMapYStrip_->GetXaxis()->FindBin(y);
      
      int numPoints = nz+1;
      boost::shared_array<double> wp(new double[numPoints]);
      for (int k=0; k<numPoints; k++) {
        double z = CCEMapYStrip_->GetYaxis()->GetBinLowEdge(k+1);
        int binz = WPMapYStrip_->GetYaxis()->FindBin(z);
        wp[k] = WPMapYStrip_->GetBinContent(biny, binz);
      }
      setWeightingPotential(isUpSideYStrip(), wp, numPoints);
      
      for (int iz=1; iz<=nz; iz++) {
        double z = CCEMapYStrip_->GetYaxis()->GetBinCenter(iz);
        double cce = calculateCCE(z);
        CCEMapYStrip_->SetBinContent(iy, iz, cce);
      }
    }
  }

  if (isUsingSymmetry()) {
    for (int iy=1; iy<=ny; iy++) {
      for (int iz=1; iz<=nz; iz++) {
        if (iy>(ny+1)/2) {
          double cce = CCEMapYStrip_->GetBinContent(ny-iy+1, iz);
          CCEMapYStrip_->SetBinContent(iy, iz, cce);
        }
      }
    }
  }

  std::cout << std::endl;
  std::cout << "Charge collection efficiency map is built." << std::endl;
}

void SimDetectorUnit2DStrip::printSimulationParameters(std::ostream& os) const
{
  os << "<SimDetectorUnit2DStrip>" << '\n'
     << "Upside anode   : " << isUpSideAnode() << '\n'
     << "Upside x-strip : " << isUpSideXStrip() << '\n'
     << std::endl;
  DeviceSimulation::printSimulationParameters(os);
}

} /* namespace comptonsoft */
