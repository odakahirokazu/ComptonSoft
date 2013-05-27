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

#include "HistPI.hh"
#include "globals.hh"
#include "OneASICData.hh"

using namespace comptonsoft;
using namespace anl;

HistPI::HistPI()
  : m_NBin(1440), m_PIMin(-10.0), m_PIMax(710.0)
{
}


ANLStatus HistPI::mod_his()
{
  VCSModule::mod_his();
  mkdir_module();

  char name[256];
  char title[256];
  /*
    const int NBIN = 1400;
    const double EMIN = 0.0; // keV
    const double EMAX = 700.0; // keV
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
	sprintf(name, "Spectrum_%02d_%d_%02d", (*itDet)->getID(), j, k);
	sprintf(title, "Spectrum_%02d_%d_%02d", (*itDet)->getID(), j, k);
        TH1* his = new TH1I(name, title, m_NBin, m_PIMin, m_PIMax);
	m_hisPI.push_back(his);
      }
      j++;
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}


ANLStatus HistPI::mod_ana()
{
  std::vector<TH1*>::iterator itHist = m_hisPI.begin();
 
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      for (int k=0; k<nCh; k++) {
	double pi = (*itChip)->getPI(k);
	(*itHist)->Fill(pi/keV);
	itHist++;
      }
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}
