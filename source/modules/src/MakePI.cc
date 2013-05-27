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

#include "MakePI.hh"

#include <iostream>

#include "TFile.h"
#include "TSpline.h"
#include "OneASICData.hh"

using namespace comptonsoft;
using namespace anl;


MakePI::MakePI()
  : m_CalFileName("calibration.root"), m_CalFile(0)
{
}


ANLStatus MakePI::mod_startup()
{
  register_parameter(&m_CalFileName, "Calibration file name");
  return AS_OK;
}


ANLStatus MakePI::mod_init()
{ 
  VCSModule::mod_init();

  bool gainCorrection(true);
  if (m_CalFileName[0]=='0') {
    gainCorrection = false;
    std::cout << "Gain correction off" << std::endl;
  }
  else {
    m_CalFile = new TFile(m_CalFileName.c_str());
  }
  
  char name[256];
  
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    int j=0;
    while ( itChip != itChipEnd ) {
      int nCh = (*itChip)->NumChannel();
      for (int k=0; k<nCh; k++) {
        if (gainCorrection) {
          sprintf(name, "CalSpline_%02d_%d_%02d", (*itDet)->getID(), j, k);
          const TSpline* spline = static_cast<const TSpline*>( m_CalFile->Get(name) );
          if (spline == 0) {
	    // std::cout << "MakePI: cal spline is not found : " << name << std::endl;
	    // return AS_QUIT;
	    (*itChip)->registerCalSpline(k, 0);
          }
	  else {
	    (*itChip)->registerCalSpline(k, spline);
	  }
        }
        else {
          (*itChip)->registerCalSpline(k, 0);
        }

      }
      j++;
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}


ANLStatus MakePI::mod_ana()
{
  std::vector<RealDetectorUnit*>::iterator itDet = GetDetectorVector().begin();
  std::vector<RealDetectorUnit*>::iterator itDetEnd = GetDetectorVector().end();
  while ( itDet != itDetEnd ) {
    std::vector<OneASICData*>::iterator itChip = (*itDet)->ASICDataBegin();
    std::vector<OneASICData*>::iterator itChipEnd = (*itDet)->ASICDataEnd();
    while ( itChip != itChipEnd ) {
      bool cal = (*itChip)->convertPHA2PI();
      if (cal == false) {
	std::cout << "MakePI: calibration return status : false" << std::endl;
	return AS_QUIT;
      }
      itChip++;
    }
    itDet++;
  }

  return AS_OK;
}


ANLStatus MakePI::mod_exit()
{
  if (m_CalFile) {
    m_CalFile->Close();
    delete m_CalFile;
    m_CalFile = 0;
  }
  return AS_OK;
}
