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

#include "VDeviceSimulation.hh"

#include "CLHEP/Random/RandGauss.h"
#include "cs_globals.hh"
#include "DetectorHit.hh"
#include "HXISGDUtility.hh"
#include "HXISGDFlagDef.hh"


namespace comptonsoft {

VDeviceSimulation::VDeviceSimulation()
  : m_QuenchingFactor(0.5),
    m_TableInitialized(false),
    m_TimeResolutionFast(100.0*ns),
    m_TimeResolutionSlow(2000.0*ns),
    m_RemoveBadChannel(true),
    m_ThresholdEnergyX(10.0*keV),
    m_ThresholdEnergyY(10.0*keV),
    m_DetectorUnit(0)
{
  m_BadChannelTable.clear();
  m_NoiseFWHMParam0.clear();
  m_NoiseFWHMParam1.clear();
  m_NoiseFWHMParam2.clear();

  m_RawHits.clear();
  m_SimulatedHits.clear();
}


VDeviceSimulation::~VDeviceSimulation()
{
  m_RawHits.clear();
  m_SimulatedHits.clear();
  m_BadChannelTable.clear();
  m_NoiseFWHMParam0.clear();
  m_NoiseFWHMParam1.clear();
  m_NoiseFWHMParam2.clear();
  
  m_DetectorUnit = 0;
}


void VDeviceSimulation::initializeTable()
{
  m_BadChannelTable.assign(SizeOfTable(), 0);
  m_NoiseFWHMParam0.assign(SizeOfTable(), 0.0);
  m_NoiseFWHMParam1.assign(SizeOfTable(), 0.0);
  m_NoiseFWHMParam2.assign(SizeOfTable(), 0.0);

  m_TableInitialized = true;
}


int VDeviceSimulation::makeDetectorHits()
{
  m_DetectorUnit->clearDetectorHits();
  simulateDetectorHits();

  while (!m_SimulatedHits.empty()) {
    m_DetectorUnit->insertDetectorHit( m_SimulatedHits.front() );
    m_SimulatedHits.pop_front();
  }
  
  return m_DetectorUnit->NumDetectorHits();
}


int VDeviceSimulation::makeRawDetectorHits()
{
  m_DetectorUnit->clearDetectorHits();
  std::vector<DetectorHit_sptr>::iterator it = m_RawHits.begin();
  for ( ; it!=m_RawHits.end(); ++it) {
    m_DetectorUnit->insertDetectorHit( *it );
  }
  return m_DetectorUnit->NumDetectorHits();
}


void VDeviceSimulation::simulateDetectorHits()
{
  simulatePulseHeights();
  removeHitsOutOfStripRange();
  sumSimulatedHits();
  
  std::list<DetectorHit_sptr>::iterator it = m_SimulatedHits.begin();
  while (it != m_SimulatedHits.end()) {
    (*it)->setPI( calculatePI((*it)->getPHA(), (*it)->getStrip()) );
    if ( BadChannel((*it)->getStrip()) ) {
      if (getRemoveBadChannel()) {
	it = m_SimulatedHits.erase(it);
	continue;
      }
      else {
	(*it)->addFlag(comptonsoft::BAD_CHANNEL);
      }
    }

    double threshold = ThresholdEnergy((*it)->getStrip());
    if ( (*it)->getPI() < threshold ) {
      it = m_SimulatedHits.erase(it);
      continue;
    }
 
    ++it;
  }
}


void VDeviceSimulation::fillStrip(DetectorHit_sptr hit)
{
  double localposx = hit->getLocalPosX();
  double localposy = hit->getLocalPosY();
  StripPair sp = m_DetectorUnit->StripXYByLocalPosition(localposx, localposy);
  hit->setStrip(sp.X(), sp.Y());
}


void VDeviceSimulation::sumSimulatedHits()
{
  std::list<DetectorHit_sptr>::iterator it1;
  std::list<DetectorHit_sptr>::iterator it2;
  
  for (it1 = m_SimulatedHits.begin(); it1 != m_SimulatedHits.end(); ++it1) {
    it2 = it1;
    ++it2;

    while ( it2 != m_SimulatedHits.end() ) {
      if ( **it2 == **it1 ) {
	**it1 += **it2;
	it2 = m_SimulatedHits.erase(it2);
      }
      else {
	++it2;
      }
    }
  }
}


void VDeviceSimulation::sumSimulatedHitsIfCoincident(double time_width)
{
  std::list<DetectorHit_sptr>::iterator it1;
  std::list<DetectorHit_sptr>::iterator it2;
  
  for (it1 = m_SimulatedHits.begin(); it1 != m_SimulatedHits.end(); ++it1) {
    it2 = it1;
    ++it2;

    while ( it2 != m_SimulatedHits.end() ) {
      double hit1Time = (*it1)->getTime();
      double hit2Time = (*it2)->getTime();
      if ( **it2 == **it1 && std::abs(hit1Time-hit2Time) <= time_width ) {
	**it1 += **it2;
        it2 = m_SimulatedHits.erase(it2);
      }
      else {
	++it2;
      }
    }
  }
}


void VDeviceSimulation::removeHitsOutOfStripRange()
{
  std::list<DetectorHit_sptr>::iterator it = m_SimulatedHits.begin();
  while (it != m_SimulatedHits.end()) {
    if ( RangeCheck((*it)->getStrip()) ) {
      ++it;
    }
    else {
      it = m_SimulatedHits.erase(it);
    }
  }
}


double VDeviceSimulation::calculatePHA(const StripPair& , double edep, double , double , double )
{
  double pha = edep;
  return pha;
}


void VDeviceSimulation::setBadChannelAll(int val)
{
  int n = m_BadChannelTable.size();
  for (int i=0; i<n; i++ ) {
    m_BadChannelTable[i] = val;
  }
}


void VDeviceSimulation::setNoiseFWHMAll(double param0, double param1, double param2)
{
  int n = m_NoiseFWHMParam0.size();
  for (int i=0; i<n; i++ ) {
    m_NoiseFWHMParam0[i] = param0;
    m_NoiseFWHMParam1[i] = param1;
    m_NoiseFWHMParam2[i] = param2;
  }
}


void VDeviceSimulation::initializeTimingProcess()
{
  m_DetectorUnit->clearDetectorHits();
 
  simulatePulseHeights();
  removeHitsOutOfStripRange();

  const double SMALL_TIME = 1.0e-15 * second;
  sumSimulatedHitsIfCoincident(SMALL_TIME);
  sortSimulatedHitsInTimeOrder();
  sumSimulatedHitsIfCoincident(m_TimeResolutionFast);
}


void VDeviceSimulation::sortSimulatedHitsInTimeOrder()
{
  m_SimulatedHits.sort( comptonsoft::time_order );
}


double VDeviceSimulation::FirstEventTime()
{
  if (m_SimulatedHits.empty()) { return 0.0; }
  return m_SimulatedHits.front()->getTime();
}


int VDeviceSimulation::makeDetectorHitsAtTime(double time_start, int time_group)
{
  const double time_end = time_start + m_TimeResolutionSlow;

  std::list<DetectorHit_sptr>::iterator it1 = m_SimulatedHits.begin();
  std::list<DetectorHit_sptr>::iterator itStart = m_SimulatedHits.begin();
  std::list<DetectorHit_sptr>::iterator itEnd = m_SimulatedHits.end();

  while (it1 != m_SimulatedHits.end()) {
    const double hitTime = (*it1)->getTime();
    if (hitTime < time_start) {
      ++it1;
      continue;
    }
    else {
      itStart = it1;
      break;
    }
  }

  while (it1 != m_SimulatedHits.end()) {
    const double hitTime = (*it1)->getTime();
    if (hitTime <= time_end) {
      (*it1)->setTimeGroup(time_group);
      ++it1;
      continue;
    }
    else {
      itEnd = it1;
      break;
    }
  }

  std::list<DetectorHit_sptr>::iterator it2;
  for (it1 = itStart; it1 != itEnd; ++it1) {
    it2 = it1;
    ++it2;
    while ( it2 != itEnd ) {
      if ( **it2 == **it1 ) {
	**it1 += **it2;
	it2 = m_SimulatedHits.erase(it2);
      }
      else {
	++it2;
      }
    }
  }

  it1 = itStart;
  while (it1 != itEnd) {
    (*it1)->setTimeGroup(time_group);
    (*it1)->setPI( calculatePI((*it1)->getPHA(), (*it1)->getStrip()) );

    if ( BadChannel((*it1)->getStrip()) ) {
      if (getRemoveBadChannel()) {
        it1 = m_SimulatedHits.erase(it1);
        continue;
      }
      else {
        (*it1)->addFlag(comptonsoft::BAD_CHANNEL);
      }
    }
    
    double threshold = ThresholdEnergy((*it1)->getStrip());
    if ( (*it1)->getPI() < threshold ) {
      it1 = m_SimulatedHits.erase(it1);
      continue;
    }
    
    m_DetectorUnit->insertDetectorHit(*it1);
    it1 = m_SimulatedHits.erase(it1);
  }

  return m_DetectorUnit->NumDetectorHits();  
}


double VDeviceSimulation::calculatePI(double pha, const StripPair& sp)
{
  // noise*noise = param0^2 + (param1*energy^(1/2))^2 + (param2*energy)^2
  // energy [keV]
  
  double ene = pha/keV; // keV
  double noiseA1 = NoiseFWHMParam0(sp); // keV
  double noiseB2 = NoiseFWHMParam1(sp) * NoiseFWHMParam1(sp) * ene; // keV2
  double noiseC1 = NoiseFWHMParam2(sp) * ene; // keV
  
  double fwhm2 = noiseA1*noiseA1 + noiseB2 + noiseC1*noiseC1; // keV
  double sigma = std::sqrt(fwhm2)/2.3548 * keV;

  double ePI = CLHEP::RandGauss::shoot(pha, sigma);
  return ePI;
}

}
