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

#include "DetectorHit.hh"
#include "FlagDefinition.hh"

namespace comptonsoft {

#if DetectorHit_BoostPool
boost::pool<> DetectorHitAllocator(sizeof(DetectorHit));
#endif

DetectorHit::~DetectorHit() = default;

DetectorHit& DetectorHit::merge(const DetectorHit& r)
{
  const double edep0 = EnergyDeposit();
  const double edep1 = r.EnergyDeposit();
  if (edep0>0.0 && edep1>0.0) {
    const double sumE = edep0 + edep1;
    setPosition( (Position()*edep0 + r.Position()*edep1)/sumE );
    setLocalPosition( (LocalPosition()*edep0 + r.LocalPosition()*edep1)/sumE );
    setRealPosition( (RealPosition()*edep0 + r.RealPosition()*edep1)/sumE );
  }
  else if (edep1 > 0.0) {
    setPosition(r.Position());
    setLocalPosition(r.LocalPosition());
    setRealPosition(r.RealPosition());
  }
  
  setEnergyDeposit(EnergyDeposit()+r.EnergyDeposit());
  setEnergyCharge(EnergyCharge()+r.EnergyCharge());
  setEnergy(Energy()+r.Energy());
  setPHA(PHA()+r.PHA());
  setEPI(EPI()+r.EPI());

  if (Time() > r.Time()) { setTime(r.Time()); }
  
  addProcess(r.Process());
  addFlags(r.Flags());
  
  return *this;
}

bool DetectorHit::isAdjacent(const DetectorHit& r, bool contact) const
{
  if (isInSameDetector(r)) {
    if (isPixel() && r.isPixel()) {
      const int dx = Pixel().X() - r.Pixel().X();
      const int dy = Pixel().Y() - r.Pixel().Y();
      if (contact) {
        return (dx==0 && (dy==1 || dy==-1)) || (dy==0 && (dx==1 || dx==-1));
      }
      else {
        if (dx!=0 || dy!=0) {
          return (-1<=dx && dx<=1) && (-1<=dy && dy<=1);
        }
      }
    }
    else if (isXStrip() && r.isXStrip()) {
      const int dx = Pixel().X() - r.Pixel().X();
      return dx==1 || dx==-1;
    }
    else if (isYStrip() && r.isYStrip()) {
      const int dy = Pixel().Y() - r.Pixel().Y();
      return dy==1 || dy==-1;
    }
  }
  return false;
}

DetectorHit& DetectorHit::mergeAdjacentSignal(const DetectorHit& r,
                                              MergedPosition mergedPosition,
                                              bool setClusteredFlag)
{
  const double epi0 = EPI();
  const double epi1 = r.EPI();
 
  if (epi0>0.0 && epi1>0.0) {
    if (mergedPosition==MergedPosition::KeepLeft) {
    }
    else if (mergedPosition==MergedPosition::Midpoint) {
      setPosition( 0.5*(Position()+r.Position()) );
      setLocalPosition( 0.5*(LocalPosition()+r.LocalPosition()) );
      setRealPosition( 0.5*(RealPosition()+r.RealPosition()) );
      if (epi0 < epi1) { setPixel(r.Pixel()); }
    }
    else if (mergedPosition==MergedPosition::EnergyWeighted) {
      const double sumE = epi0 + epi1;
      setPosition( (Position()*epi0+r.Position()*epi1)/sumE );
      setLocalPosition( (LocalPosition()*epi0 + r.LocalPosition()*epi1)/sumE );
      setRealPosition( (RealPosition()*epi0 + r.RealPosition()*epi1)/sumE );
      if (epi0 < epi1) { setPixel(r.Pixel()); }
    }
    else if (mergedPosition==MergedPosition::CopyRight) {
      setPosition(r.Position());
      setLocalPosition(r.LocalPosition());
      setRealPosition(r.RealPosition());
      setPixel(r.Pixel());
    }
  }

  setEnergyDeposit(EnergyDeposit()+r.EnergyDeposit());
  setEnergyCharge(EnergyCharge()+r.EnergyCharge());
  setEnergy(Energy()+r.Energy());
  setPHA(PHA()+r.PHA());
  setEPI(EPI()+r.EPI());

  if (Time() > r.Time()) { setTime(r.Time()); }

  addProcess(r.Process());
  addFlags(r.Flags());
  if (setClusteredFlag) {
    addFlags(flag::AdjacentClustered);
  }
  return *this;
}

} /* namespace comptonsoft */
