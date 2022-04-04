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

#include "SimDetectorUnit2DPixel.hh"

#include <iostream>
#include <algorithm>
#include <memory>
#include <cmath>
#include <boost/format.hpp>

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"
#include "TRandom3.h"
#include "TH3.h"

#include "AstroUnits.hh"
#include "FlagDefinition.hh"
#include "DetectorHit.hh"
#include "WeightingPotentialPixel.hh"

namespace unit = anlgeant4::unit;

namespace comptonsoft {

SimDetectorUnit2DPixel::SimDetectorUnit2DPixel()
  : usingSymmetry_(true),
    CCEMap_(nullptr),
    WPMap_(nullptr)
{
}

SimDetectorUnit2DPixel::~SimDetectorUnit2DPixel() = default;

void SimDetectorUnit2DPixel::initializeEvent()
{
  RealDetectorUnit2DPixel::initializeEvent();
  DeviceSimulation::initializeEvent();
}

void SimDetectorUnit2DPixel::simulatePulseHeights()
{
  const int N = NumberOfRawHits();
  for (int i=0; i<N; i++) {
    DetectorHit_sptr rawhit = getRawHit(i);
    const double edep = rawhit->EnergyDeposit();
    const double localposx = rawhit->LocalPositionX();
    const double localposy = rawhit->LocalPositionY();
    const double localposz = rawhit->LocalPositionZ();

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

    const PixelID pixel = findPixel(localposx, localposy);
    
    if (DiffusionMode() == 0) {
      DetectorHit_sptr hit = generateHit(*rawhit, pixel);
      insertSimulatedHit(hit);
    }
    else {
      DetectorHit_sptr hit = generateHit(*rawhit, pixel);
      const double energyCharge = hit->EnergyCharge();

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
        const double dx = gRandom->Gaus(0.0, diffusionSigma);
        const double dy = gRandom->Gaus(0.0, diffusionSigma);
        const PixelID pixelDiff = findPixel(localposx+dx, localposy+dy);

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
    if (ChargeCollectionMode()>=3) {
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

DetectorHit_sptr SimDetectorUnit2DPixel::generateHit(const DetectorHit& rawhit,
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

  applyQuenching(hit);

  const double edep = hit->EnergyDeposit();
  const double localposx = hit->LocalPositionX();
  const double localposy = hit->LocalPositionY();
  const double localposz = hit->LocalPositionZ();
  const double energyCharge = calculateEnergyCharge(pixel,
                                                    edep,
                                                    localposx,
                                                    localposy,
                                                    localposz);
  hit->setEnergyCharge(energyCharge);

  return hit;
}

double SimDetectorUnit2DPixel::
ChargeCollectionEfficiency(const PixelID& pixel,
                           double x,
                           double y,
                           double z) const
{
  double xPixel, yPixel, zPixel;
  LocalPosition(pixel, &xPixel, &yPixel, &zPixel);
  
  const double xInPixel = x - xPixel;
  const double yInPixel = y - yPixel;
  
  const int ix = CCEMap_->GetXaxis()->FindBin(xInPixel/unit::cm);
  const int iy = CCEMap_->GetYaxis()->FindBin(yInPixel/unit::cm);
  const int iz = CCEMap_->GetZaxis()->FindBin(z/unit::cm);

  return CCEMap_->GetBinContent(ix, iy, iz);
}

double SimDetectorUnit2DPixel::WeightingPotential(const PixelID& pixel,
                                                  double x, double y, double z)
{
  double xPixel, yPixel, zPixel;
  Position(pixel, &xPixel, &yPixel, &zPixel);
  
  const double xInPixel = x - xPixel;
  const double yInPixel = y - yPixel;
  
  const int ix = WPMap_->GetXaxis()->FindBin(xInPixel/unit::cm);
  const int iy = WPMap_->GetYaxis()->FindBin(yInPixel/unit::cm);
  const int iz = WPMap_->GetZaxis()->FindBin(z/unit::cm);

  return WPMap_->GetBinContent(ix, iy, iz);
}

bool SimDetectorUnit2DPixel::isCCEMapPrepared()
{
  return (getCCEMap()!=0);
}

void SimDetectorUnit2DPixel::buildWPMap()
{
  if (ChargeCollectionMode() == 2) {
    buildWPMap(31, 31, 64, 1.0);
  }
  else if (ChargeCollectionMode() == 3) {
    buildWPMap(93, 93, 64, 3.0);
  }
  else {
    std::cout << "Error : buildWPMap() " << std::endl;
    return;
  }
}

void SimDetectorUnit2DPixel::buildWPMap(int nx, int ny, int nz,
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
  
  const std::string histname = (boost::format("wp_%04d")%getID()).str();
  WPMap_ =  new TH3D(histname.c_str(), histname.c_str(),
                     nx, -0.5*MapSizeX/unit::cm, +0.5*MapSizeX/unit::cm,
                     ny, -0.5*MapSizeY/unit::cm, +0.5*MapSizeY/unit::cm,
                     nz, -0.5*MapSizeZ/unit::cm, +0.5*MapSizeZ/unit::cm);
 
  auto wpModel = std::make_unique<WeightingPotentialPixel>();
  wpModel->setGeometry(getPixelPitchX(), getPixelPitchY(), getThickness(),
                       NumPixelsInWPCalculation());
  wpModel->initializeTable();
  std::cout << "calculating weighing potential..." << std::endl;
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    const double x = WPMap_->GetXaxis()->GetBinCenter(ix) * unit::cm;
    for (int iy=1; iy<=ny; iy++) {
      const double y = WPMap_->GetYaxis()->GetBinCenter(iy) * unit::cm;
      wpModel->setXY(x, y);
      for (int iz=1; iz<=nz; iz++) {
        const double z = WPMap_->GetZaxis()->GetBinCenter(iz) * unit::cm;
        if (isUpSideReadout()) {
          double wp = 0.;
          if (iz==nz) {
            if (std::abs(x)<0.5*getPixelPitchX() && std::abs(y)<0.5*getPixelPitchY()) {
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
          WPMap_->SetBinContent(ix, iy, iz, wp);
        }
        else {
          double wp = 0.;
          if (iz==1) {
            if (std::abs(x)<0.5*getPixelPitchX() && std::abs(y)<0.5*getPixelPitchY()) {
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
          WPMap_->SetBinContent(ix, iy, iz, wp);
        }
      }
    }
  }
  std::cout << std::endl;
}

void SimDetectorUnit2DPixel::buildCCEMap()
{
  if (ChargeCollectionMode() == 2) {
    buildCCEMap(31, 31, 63, 1.0);
  }
  else if (ChargeCollectionMode() == 3) {
    buildCCEMap(93, 93, 63, 3.0);
  }
  else {
    std::cout << "Error : buildCCEMap() " << std::endl;
    return;
  }
}

void SimDetectorUnit2DPixel::buildCCEMap(int nx, int ny, int nz,
                                         double pixel_factor)
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
  
  const std::string histname = (boost::format("cce_%04d")%getID()).str();
  CCEMap_ =  new TH3D(histname.c_str(), histname.c_str(),
                      nx, -0.5*MapSizeX/unit::cm, +0.5*MapSizeX/unit::cm,
                      ny, -0.5*MapSizeY/unit::cm, +0.5*MapSizeY/unit::cm,
                      nz, -0.5*MapSizeZ/unit::cm, +0.5*MapSizeZ/unit::cm);
  
  std::cout << "calculating charge collection efficiency..." << std::endl;
  
  for (int ix=1; ix<=nx; ix++) {
    std::cout << '*' << std::flush;
    for (int iy=1; iy<=ny; iy++) {
      if (!isUsingSymmetry() || (ix<=(nx+1)/2 && iy<=(ny+1)/2)) {
        const double x_in_cm = CCEMap_->GetXaxis()->GetBinCenter(ix);
        const double y_in_cm = CCEMap_->GetYaxis()->GetBinCenter(iy);
        const int binx = WPMap_->GetXaxis()->FindBin(x_in_cm);
        const int biny = WPMap_->GetYaxis()->FindBin(y_in_cm);

        const int numPoints = nz+1;
        boost::shared_array<double> wp(new double[numPoints]);
        for (int k=0; k<numPoints; k++) {
          const double z_in_cm = CCEMap_->GetZaxis()->GetBinLowEdge(k+1);
          const int binz = WPMap_->GetZaxis()->FindBin(z_in_cm);
          wp[k] = WPMap_->GetBinContent(binx, biny, binz);
        }
        setWeightingPotential(isUpSideReadout(), wp, numPoints);

        for (int iz=1; iz<=nz; iz++) {
          const double z = CCEMap_->GetZaxis()->GetBinCenter(iz) * unit::cm;
          const double cce = calculateCCE(z);
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
            const double cce = CCEMap_->GetBinContent(nx-ix+1, iy, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
          else if (ix<=(nx+1)/2 && iy>(ny+1)/2) {
            const double cce = CCEMap_->GetBinContent(ix, ny-iy+1, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
          else if (ix>(ny+1)/2 && iy>(ny+1)/2) {
            const double cce = CCEMap_->GetBinContent(nx-ix+1, ny-iy+1, iz);
            CCEMap_->SetBinContent(ix, iy, iz, cce);
          }
        }
      }
    }
  }

  std::cout << std::endl;
  std::cout << "Charge collection efficiency map is built." << std::endl;
}

void SimDetectorUnit2DPixel::printSimulationParameters(std::ostream& os) const
{
  os << "<SimDetectorUnit2DPixel>" << '\n'
     << "Upside anode   : " << isUpSideAnode() << '\n'
     << "Upside pixel   : " << isUpSideReadout() << '\n'
     << std::endl;
  DeviceSimulation::printSimulationParameters(os);
}
  
} /* namespace comptonsoft */
