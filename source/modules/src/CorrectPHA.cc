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

// 2009-06-22 Hirokazu Odaka
// 2007-12-xx Hirokazu Odaka

#include "CorrectPHA.hh"
#include "TFile.h"
#include "TH1.h"
#include "OneASICData.hh"

using namespace comptonsoft;
using namespace anl;


CorrectPHA::CorrectPHA()
  : m_PedestalFileName("pedestal.root"),
    m_CMNByMedian(true)
{
}


ANLStatus CorrectPHA::mod_startup()
{
  register_parameter(&m_PedestalFileName, "pedestal_data");
  set_parameter_description("ROOT file of pedestal levels.");
  register_parameter(&m_CMNByMedian, "median_CMN_estimation");
  set_parameter_description("Calculation method of common mode noise values. Median (1) or mean (0).");
  
  return AS_OK;
}


ANLStatus CorrectPHA::mod_init()
{
  VCSModule::mod_init();

  TFile* pedestalFile = new TFile(m_PedestalFileName.c_str());
  char name[256];
  
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    int j=0;
    while ( itChip != itChipEnd ) {
      sprintf(name, "pedestal/pedestal_%02d_%d", (*itDet)->getID(), j);
      TH1D* pedestal = dynamic_cast<TH1D*>( pedestalFile->Get(name) );
      int nCh = (*itChip)->NumChannel();
      for (int k=0; k<nCh; k++) {
	(*itChip)->setPedestal(k, pedestal->GetBinContent(k+1));
      }
      j++;
      itChip++;
    }
    itDet++;
  }

  pedestalFile->Close();
  delete pedestalFile;

  return AS_OK;
}


ANLStatus CorrectPHA::mod_ana()
{
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      if (m_CMNByMedian) {
        (*itChip)->calculateCommonModeNoiseByMedian();
      }
      else {
        (*itChip)->calculateCommonModeNoiseByMean();
      }
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}
