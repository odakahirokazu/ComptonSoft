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

#include "CalcPedestal.hh"
#include "OneASICData.hh"

using namespace comptonsoft;
using namespace anl;

CalcPedestal::CalcPedestal()
  : m_NBin(1024), m_ADCMin(-0.5), m_ADCMax(1023.5)
{
}


ANLStatus CalcPedestal::mod_his()
{
  VCSModule::mod_his();
  mkdir_module("pedestal");
  
  char name[256];
  char title[256];
  /*
    const int NBIN = 4096;
    const double ADCMIN = -0.5;
    const double ADCMAX = +4095.5;
  */
  
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();

  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    int j=0;
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      sprintf(name, "pedestal_%02d_%d", (*itDet)->getID(), j);
      sprintf(title, "pedestal_%02d_%d", (*itDet)->getID(), j);
      TH1* hisPed = new TH1D(name, title,
			     nCh, -0.5, static_cast<double>(nCh)-0.5);
      m_hisPed.push_back(hisPed);

      sprintf(name, "noiselevel_%02d_%d", (*itDet)->getID(), j);
      sprintf(title, "noiselevel_%02d_%d", (*itDet)->getID(), j);
      TH1* hisNoise = new TH1D(name, title, nCh, -0.5, static_cast<double>(nCh)-0.5);
      m_hisNoise.push_back(hisNoise);

      for (int k=0; k<nCh; k++) {
	sprintf(name, "spectrum_%02d_%d_%02d", (*itDet)->getID(), j, k);
	sprintf(title, "spectrum_%02d_%d_%02d", (*itDet)->getID(), j, k);
        TH1* hisSpec = new TH1I(name, title, m_NBin, m_ADCMin, m_ADCMax);
	m_hisSpec.push_back(hisSpec);
      }
      j++;
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}


ANLStatus CalcPedestal::mod_ana()
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
	int rawadc = (*itChip)->getRawADC(k);
	(*itHist)->Fill(rawadc);
	itHist++;
      }
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}


ANLStatus CalcPedestal::mod_endrun()
{
  const int ADCZeroLevelBin = 250;  // 0;    // 2048;
  const int SearchHalfWidth = 245;  // 1024; // 2004;

  const int SearchMin = ADCZeroLevelBin - SearchHalfWidth;
  const int SearchMax = ADCZeroLevelBin + SearchHalfWidth;
  const int RangeHalfWidth = 8;

  std::vector<TH1*>::iterator itHisSpec = m_hisSpec.begin();
  std::vector<TH1*>::iterator itHisPed = m_hisPed.begin();
  std::vector<TH1*>::iterator itHisNoise = m_hisNoise.begin();

  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    int j=0;
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      for (int k=0; k<nCh; k++) {
	(*itHisSpec)->GetXaxis()->SetRange(SearchMin, SearchMax);
	int centerbin = (*itHisSpec)->GetMaximumBin();
	int minbin = centerbin - RangeHalfWidth;
	int maxbin = centerbin + RangeHalfWidth;
	(*itHisSpec)->GetXaxis()->SetRange(minbin, maxbin);
	double pedestalMean = (*itHisSpec)->GetMean();
	(*itHisPed)->SetBinContent(k+1, pedestalMean);

	(*itHisSpec)->GetXaxis()->SetRange();
	double pedestalRMS = (*itHisSpec)->GetRMS();
	(*itHisNoise)->SetBinContent(k+1, pedestalRMS);
	*itHisSpec++;
      }
      itHisPed++;
      itHisNoise++;
      j++;
      itChip++;
    }
    itDet++;
  }
 
  return AS_OK;
}
