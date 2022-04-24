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
#include <cmath>
#include <boost/format.hpp>
#include "TRandom3.h"

#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"
#include "WeightingPotentialStrip.hh"

namespace unit = anlgeant4::unit;

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
    const double edep = rawhit->EnergyDeposit();
    const double localposx = rawhit->LocalPositionX();
    const double localposy = rawhit->LocalPositionY();
    const double localposz = rawhit->LocalPositionZ();
    const PixelID sp = findPixel(localposx, localposy);
    
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
      const PixelID xsp(sp.X(), PixelID::Undefined);
      const PixelID ysp(PixelID::Undefined, sp.Y());
      DetectorHit_sptr xhit = generateHit(*rawhit, xsp);
      DetectorHit_sptr yhit = generateHit(*rawhit, ysp);
      insertSimulatedHit(xhit);
      insertSimulatedHit(yhit);
    }
    else {
      const PixelID xsp(sp.X(), PixelID::Undefined);
      const PixelID ysp(PixelID::Undefined, sp.Y());
      DetectorHit_sptr xhit = generateHit(*rawhit, xsp);
      DetectorHit_sptr yhit = generateHit(*rawhit, ysp);
      const double xECharge = xhit->EnergyCharge();
      const double yECharge = yhit->EnergyCharge();

      const int numDivision = DiffusionDivisionNumber();
      const double edepDivision = edep/numDivision;
      const double xEChargeDivision = xECharge/numDivision;
      const double yEChargeDivision = yECharge/numDivision;
      
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
          const double dx = gRandom->Gaus(0.0, xDiffusionSigma);
          const double dy = gRandom->Gaus(0.0, xDiffusionSigma);
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
          const double dx = gRandom->Gaus(0.0, yDiffusionSigma);
          const double dy = gRandom->Gaus(0.0, yDiffusionSigma);
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
    else if (ChargeCollectionMode()>=4) {
      const int strip_range = 7;

      for (int delta_strip=-strip_range; delta_strip<=strip_range; delta_strip++) {
        if (delta_strip!=0) {
          PixelID nearStrip(sp.X()+delta_strip, PixelID::Undefined);
          DetectorHit_sptr hit = generateHit(*rawhit, nearStrip);
          hit->setEnergyDeposit(0.0);
          insertSimulatedHit(hit);
        }
      }

      for (int delta_strip=-strip_range; delta_strip<=strip_range; delta_strip++) {
        if (delta_strip!=0) {
          PixelID nearStrip(PixelID::Undefined, sp.Y()+delta_strip);
          DetectorHit_sptr hit = generateHit(*rawhit, nearStrip);
          hit->setEnergyDeposit(0.0);
          insertSimulatedHit(hit);
        }
      }
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
                           const double x, const double y, const double z) const
{
  const vector3_t pixelCenter = LocalPosition(sp);
  
  double cce = 1.0;
  if (sp.isXStrip()) {
    const double xInPixel = x - pixelCenter.x();
    const int ix = CCEMapXStrip_->GetXaxis()->FindBin(xInPixel/unit::cm);
    const int iz = CCEMapXStrip_->GetYaxis()->FindBin(z/unit::cm);
    cce =  CCEMapXStrip_->GetBinContent(ix, iz);
  }
  else if (sp.isYStrip()) {
    const double yInPixel = y - pixelCenter.y();
    const int iy = CCEMapYStrip_->GetXaxis()->FindBin(yInPixel/unit::cm);
    const int iz = CCEMapYStrip_->GetYaxis()->FindBin(z/unit::cm);
    cce =  CCEMapYStrip_->GetBinContent(iy, iz);
  }

  return cce;
}

double SimDetectorUnit2DStrip::
WeightingPotential(const PixelID& sp,
                   const double x, const double y, const double z)
{
  const vector3_t pixelCenter = LocalPosition(sp);

  double wp = 0.0;
  if (sp.isXStrip()) {
    const double xInPixel = x - pixelCenter.x();
    const int ix = WPMapXStrip_->GetXaxis()->FindBin(xInPixel/unit::cm);
    const int iz = WPMapXStrip_->GetYaxis()->FindBin(z/unit::cm);
    wp =  WPMapXStrip_->GetBinContent(ix, iz);
  }
  else if (sp.isYStrip()) {
    const double yInPixel = y - pixelCenter.y();
    const int iy = WPMapYStrip_->GetXaxis()->FindBin(yInPixel/unit::cm);
    const int iz = WPMapYStrip_->GetYaxis()->FindBin(z/unit::cm);
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
  else if (ChargeCollectionMode() == 4) {
    buildWPMap(285, 285, 128, 15.0);
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
  
  const std::string histnameX = (boost::format("wp_%04d_x")%getID()).str();
  WPMapXStrip_ =  new TH2D(histnameX.c_str(), histnameX.c_str(),
                           nx, -0.5*MapSizeX/unit::cm, +0.5*MapSizeX/unit::cm,
                           nz, -0.5*MapSizeZ/unit::cm, +0.5*MapSizeZ/unit::cm);
  const std::string histnameY = (boost::format("wp_%04d_y")%getID()).str();
  WPMapYStrip_ =  new TH2D(histnameY.c_str(), histnameY.c_str(),
                           ny, -0.5*MapSizeY/unit::cm, +0.5*MapSizeY/unit::cm,
                           nz, -0.5*MapSizeZ/unit::cm, +0.5*MapSizeZ/unit::cm);

  std::cout << "calculating weighing potential..." << std::endl;

  auto wpModel = std::make_unique<WeightingPotentialStrip>();
  wpModel->setGeometry(getPixelPitchX(), getThickness(),
                       NumPixelsInWPCalculation());
  wpModel->initializeTable();
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    const double x = WPMapXStrip_->GetXaxis()->GetBinCenter(ix) * unit::cm;
    wpModel->setX(x);
    for (int iz=1; iz<=nz; iz++) {
      const double z = WPMapXStrip_->GetYaxis()->GetBinCenter(iz) * unit::cm;
      if (isUpSideXStrip()) {
        double wp = 0.;
        if (iz==nz) {
          if (std::abs(x)<0.5*getPixelPitchX()) {
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
          wp = wpModel->calculateWeightingPotential(z);
        }
        WPMapXStrip_->SetBinContent(ix, iz, wp);
      }
      else {
        double wp = 0.;
        if (iz==1) {
          if (std::abs(x)<0.5*getPixelPitchX()) {
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
          wp = wpModel->calculateWeightingPotential(-z);
        }
        WPMapXStrip_->SetBinContent(ix, iz, wp);
      }
    }
  }

  wpModel->setGeometry(getPixelPitchY(), getThickness(), NumPixelsInWPCalculation());
  wpModel->initializeTable();
  for (int iy=1; iy<=ny; iy++) {
    std::cout << '*' << std::flush;
    const double y = WPMapYStrip_->GetXaxis()->GetBinCenter(iy) * unit::cm;
    wpModel->setX(y);
    for (int iz=1; iz<=nz; iz++) {
      const double z = WPMapYStrip_->GetYaxis()->GetBinCenter(iz) * unit::cm;
      if (isUpSideYStrip()) {
        double wp = 0.;
        if (iz==nz) {
          if (std::abs(y)<0.5*getPixelPitchY()) {
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
          wp = wpModel->calculateWeightingPotential(z);
        }
        WPMapYStrip_->SetBinContent(iy, iz, wp);
      }
      else {
        double wp = 0.;
        if (iz==1) {
          if (std::abs(y)<0.5*getPixelPitchY()) {
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
          wp = wpModel->calculateWeightingPotential(-z);
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
  else if (ChargeCollectionMode() == 4) {
    buildCCEMap(285, 285, 127, 15.0);
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
  
  const std::string histnameX = (boost::format("cce_%04d_x")%getID()).str();
  CCEMapXStrip_ =  new TH2D(histnameX.c_str(), histnameX.c_str(),
                            nx, -0.5*MapSizeX/unit::cm, +0.5*MapSizeX/unit::cm,
                            nz, -0.5*MapSizeZ/unit::cm, +0.5*MapSizeZ/unit::cm);
  const std::string histnameY = (boost::format("cce_%04d_y")%getID()).str();
  CCEMapYStrip_ =  new TH2D(histnameY.c_str(), histnameY.c_str(),
                            nx, -0.5*MapSizeY/unit::cm, +0.5*MapSizeY/unit::cm,
                            nz, -0.5*MapSizeZ/unit::cm, +0.5*MapSizeZ/unit::cm);
  
  std::cout << "calculating charge collection efficiency..." << std::endl;
  
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    if (!isUsingSymmetry() || ix<=(nx+1)/2) {
      const double x_in_cm = CCEMapXStrip_->GetXaxis()->GetBinCenter(ix);
      const int binx = WPMapXStrip_->GetXaxis()->FindBin(x_in_cm);
      
      const int numPoints = nz+1;
      boost::shared_array<double> wp(new double[numPoints]);
      for (int k=0; k<numPoints; k++) {
        const double z_in_cm = CCEMapXStrip_->GetYaxis()->GetBinLowEdge(k+1);
        const int binz = WPMapXStrip_->GetYaxis()->FindBin(z_in_cm);
        wp[k] = WPMapXStrip_->GetBinContent(binx, binz);
      }
      setWeightingPotential(isUpSideXStrip(), wp, numPoints);
      
      for (int iz=1; iz<=nz; iz++) {
        const double z = CCEMapXStrip_->GetYaxis()->GetBinCenter(iz) * unit::cm;
        const double cce = calculateCCE(z);
        // std::cout << cce << std::endl;
        CCEMapXStrip_->SetBinContent(ix, iz, cce);
      }
    }
  }

  if (isUsingSymmetry()) {
    for (int ix=1; ix<=nx; ix++) {
      for (int iz=1; iz<=nz; iz++) {
        if (ix>(nx+1)/2) {
          const double cce = CCEMapXStrip_->GetBinContent(nx-ix+1, iz);
          CCEMapXStrip_->SetBinContent(ix, iz, cce);
        }
      }
    }
  }

  for (int iy=1; iy<=ny; iy++) {
    std::cout << '*' << std::flush;
    if (!isUsingSymmetry() || iy<=(ny+1)/2) {
      const double y_in_cm = CCEMapYStrip_->GetXaxis()->GetBinCenter(iy);
      const int biny = WPMapYStrip_->GetXaxis()->FindBin(y_in_cm);
      
      const int numPoints = nz+1;
      boost::shared_array<double> wp(new double[numPoints]);
      for (int k=0; k<numPoints; k++) {
        const double z_in_cm = CCEMapYStrip_->GetYaxis()->GetBinLowEdge(k+1);
        const int binz = WPMapYStrip_->GetYaxis()->FindBin(z_in_cm);
        wp[k] = WPMapYStrip_->GetBinContent(biny, binz);
      }
      setWeightingPotential(isUpSideYStrip(), wp, numPoints);
      
      for (int iz=1; iz<=nz; iz++) {
        const double z = CCEMapYStrip_->GetYaxis()->GetBinCenter(iz) * unit::cm;
        const double cce = calculateCCE(z);
        CCEMapYStrip_->SetBinContent(iy, iz, cce);
      }
    }
  }

  if (isUsingSymmetry()) {
    for (int iy=1; iy<=ny; iy++) {
      for (int iz=1; iz<=nz; iz++) {
        if (iy>(ny+1)/2) {
          const double cce = CCEMapYStrip_->GetBinContent(ny-iy+1, iz);
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
