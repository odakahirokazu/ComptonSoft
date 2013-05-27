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

#include "HistPHA.hh"
#include "OneASICData.hh"

using namespace comptonsoft;
using namespace anl;

HistPHA::HistPHA()
  : m_NBin(1280), m_PHAMin(-256.5), m_PHAMax(1023.5)
{
}


ANLStatus HistPHA::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();

  char name[256];
  char title[256];
  /*
    const int NBIN = 4096;
    const double PHAMIN = -2048.5;
    const double PHAMAX = +2047.5;
  */
  
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    int j=0;
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      for (int k=0; k<nCh; k++) {
	sprintf(name, "spectrum_%02d_%d_%02d", (*itDet)->getID(), j, k);
	sprintf(title, "spectrum_%02d_%d_%02d", (*itDet)->getID(), j, k);
        TH1* his = new TH1I(name, title, m_NBin, m_PHAMin, m_PHAMax);
	m_hisSpec.push_back(his);
      }
      j++;
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}


ANLStatus HistPHA::mod_ana()
{
  std::vector<TH1*>::iterator itHist = m_hisSpec.begin();
 
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      for (int k=0; k<nCh; k++) {
	double pha = (*itChip)->getPHA(k);
	(*itHist)->Fill(pha);
	itHist++;
      }
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}
