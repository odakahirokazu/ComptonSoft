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

// RealDetectorUnit class
// RealDetectorUnit.hh
//
// 2007-12-xx Hirokazu Odaka
// 2008-07-29 Hirokazu Odaka
// 2008-08-25 Hirokazu Odaka
// 2009-07-09 Hirokazu Odaka
//

#include <iostream>

#include "RealDetectorUnit.hh"

#include "ChannelTable.hh"
#include "OneASICData.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

RealDetectorUnit::RealDetectorUnit()
  : m_ChannelTable(0), m_AnalysisMode(1)
{
  m_ASICDataVector.clear();

  m_DetectorHits.clear();
  m_AnalyzedHits.clear();
}


RealDetectorUnit::~RealDetectorUnit()
{
  m_DetectorHits.clear();
  m_AnalyzedHits.clear();
}


StripPair RealDetectorUnit::convertChannel2Strip(int hit1_chip,
						 int hit1_channel,
						 int hit2_chip,
						 int hit2_channel) const
{
  if (hit1_chip<0 || hit1_channel<0) {
    return StripPair(-1, -1);
  }

  StripPair sp = m_ChannelTable->get(hit1_chip, hit1_channel);
  
  if (hit2_chip<0 || hit2_channel<0) {
    return sp;
  }
  
  StripPair sp2 = m_ChannelTable->get(hit2_chip, hit2_channel);
  return sp.Combine(sp2);
}


int RealDetectorUnit::convertChannel2StripX(int hit1_chip,
					    int hit1_channel,
					    int hit2_chip,
					    int hit2_channel) const
{
  StripPair sp
    = convertChannel2Strip(hit1_chip, hit1_channel, hit2_chip, hit2_channel);
  return sp.X();
}


int RealDetectorUnit::convertChannel2StripY(int hit1_chip,
					    int hit1_channel,
					    int hit2_chip,
					    int hit2_channel) const
{
  StripPair sp
    = convertChannel2Strip(hit1_chip, hit1_channel, hit2_chip, hit2_channel);
  return sp.Y();
}


void RealDetectorUnit::findChipAndChannel(int strip_x, int strip_y,
                                          int* pChip, int* pChannel) const
{
  StripPair sp(strip_x, strip_y);
  int nChip = NumASIC();
  for (int iChip=0; iChip<nChip; iChip++) {
    OneASICData* asic = m_ASICDataVector[iChip];
    for (int i=0; i<asic->NumChannel(); i++) {
      if (convertChannel2Strip(iChip, i) == sp) {
        *pChip = iChip;
        *pChannel = i;
        return;
      }
    }
  }
}


std::pair<int, int> RealDetectorUnit::
findChipAndChannel(int strip_x, int strip_y) const
{
  int chip = -1;
  int channel = -1;
  findChipAndChannel(strip_x, strip_y, &chip, &channel);
  return std::make_pair(chip, channel);
}


int RealDetectorUnit::selectHits()
{
  clearDetectorHits();

  const int numChip = NumASIC();
  for (int chip=0; chip<numChip; chip++) {
    OneASICData* asic = getASICData(chip);
    asic->selectHit();

    const bool anode = asic->isAnodeSide();
    const bool prioritySide = asic->isPrioritySide();
    const int nCh = asic->NumChannel();
    for (int channel=0; channel<nCh; channel++) {
      if (asic->getHitChannel(channel)) {
	DetectorHit_sptr hit(new DetectorHit);
	StripPair sp = convertChannel2Strip(chip, channel);
	
	hit->setPHA( asic->getPHA(channel) );
	hit->setPI( asic->getPI(channel) );
	hit->setDetectorID( this->getID() );
	hit->setChipID(chip);
	hit->setChannel(channel);
	hit->setStrip(sp.X(), sp.Y());

	if (anode) { hit->addFlag(ANODE_SIDE); }
	if (prioritySide) { hit->addFlag(PRIORITY_SIDE); }

	insertDetectorHit(hit);
      }
    }
  }

  return NumDetectorHits();
}


int RealDetectorUnit::discriminateHits()
{
  std::vector<DetectorHit_sptr>::iterator it = m_DetectorHits.begin();
  while (it != m_DetectorHits.end()) {
    int chip = (*it)->getChipID();
    double energy = (*it)->getPI();
    const OneASICData* asic = getASICData(chip);
    double threshold = asic->getThresholdEnergy();
    if (energy < threshold) {
      it = m_DetectorHits.erase(it);
    }
    else {
      ++it;
    }
  }
  
  return NumDetectorHits();
}


int RealDetectorUnit::analyzeHits()
{
  clearAnalyzedHits();

  std::vector<DetectorHit_sptr>::iterator it;
  std::vector<DetectorHit_sptr>::iterator itEnd = m_DetectorHits.end();

  for (it = m_DetectorHits.begin(); it != itEnd; ++it) {
    DetectorHit_sptr hit(new DetectorHit(**it));
    insertAnalyzedHit(hit);
  }
  return NumAnalyzedHits();
}


void RealDetectorUnit::applyNewPI()
{
  for (DetectorHitVIter it=m_DetectorHits.begin(); it!=m_DetectorHits.end(); ++it) {
    DetectorHit_sptr hit = *it;
    int chipid = hit->getChipID();
    int channel = hit->getChannel();
    double vPHA = hit->getPHA();
    OneASICData* chip = getASICData(chipid);
    double vPI = chip->PHA2PI(channel, vPHA);
    hit->setPI(vPI);
  }
}


void RealDetectorUnit::assignDetectorInfo()
{
  if (!m_ChannelTable) return;
  
  for (DetectorHitVIter it=m_DetectorHits.begin(); it!=m_DetectorHits.end(); ++it) {
    DetectorHit_sptr hit = *it;
    int chipid = hit->getChipID();
    int channel = hit->getChannel();
    int stripX = hit->getStripX();
    int stripY = hit->getStripY();
    if (chipid<0 || channel<0) {
      findChipAndChannel(stripX, stripY, &chipid, &channel);
      hit->setChipID(chipid);
      hit->setChannel(channel);
    }
    OneASICData* chip = getASICData(chipid);
    if (chip && chip->isAnodeSide()) {
      hit->addFlag(ANODE_SIDE);
    }
  }
}

}
