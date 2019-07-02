/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#include "AHRayTracingPrimaryGen.hh"
#include "fitsio.h"
#include "AstroUnits.hh"

using namespace anlnext;
using namespace anlgeant4;

namespace comptonsoft
{

AHRayTracingPrimaryGen::AHRayTracingPrimaryGen()
  : m_FileName("raytracing.fits"),
    m_EventNum(0),
    m_ID(0)
{
  add_alias("AHRayTracingPrimaryGen");
}

ANLStatus AHRayTracingPrimaryGen::mod_define()
{
  anlnext::ANLStatus status = BasicPrimaryGen::mod_define();
  if (status!=AS_OK) {
    return status;
  }  

  setParticleName("gamma");
  
  register_parameter(&m_FileName, "filename");
  register_parameter(&m_offset, "position_offset", unit::mm, "mm");
  register_parameter(&m_EnergyResample, "energy_resample");
  
  return AS_OK;
}

ANLStatus AHRayTracingPrimaryGen::mod_initialize()
{
  ANLStatus status = BasicPrimaryGen::mod_initialize();
  if (status!=AS_OK) {
    return status;
  }

  fitsfile* fits(0);
  int fits_status(0);
  fits_open_file(&fits, m_FileName.c_str(), READONLY, &fits_status);
  if (fits_status) {
    fits_report_error(stderr, fits_status);
    return AS_QUIT_ERROR;
  }
  
  std::string colname[NumColumns] = {"energy","x","y","xDirection","yDirection","zDirection"};
  int colid[NumColumns] = {0};
  ffmahd(fits, 2, IMAGE_HDU, &fits_status);
  if(fits_status){
    fits_report_error(stderr, fits_status);
  }
  
  for(int i=0; i<NumColumns; ++i) {
    fits_get_colnum(fits, CASEINSEN, const_cast<char*>(colname[i].c_str()),
                    &colid[i], &fits_status);
    if (fits_status) {
      fits_report_error(stderr, fits_status);
      return AS_QUIT_ERROR;
    }
  }
  
  int nfound(0), anynull(0);
  long naxes[2] = {0, 0};
  fits_read_keys_lng(fits, (char*)"NAXIS", 1, 2, naxes, &nfound, &fits_status);
  m_EventNum = static_cast<int>(naxes[1]);
  std::cout << "FITS read >> " << nfound << " " << m_EventNum << " " << naxes[0] << " " << naxes[1] << std::endl;
  
  if (fits_status) {
    fits_report_error(stderr, fits_status);
    return AS_QUIT_ERROR;
  }
  
  double doublenull(0.0);
  for(int i=0; i<NumColumns; ++i){
    std::cout << "  ** Get " << colname[i] << std::endl;
    m_Columns[i].resize(m_EventNum);
    fits_read_col(fits, TDOUBLE, colid[i], (long)1, (long)1,
                  naxes[1], &doublenull,
                  &(m_Columns[i][0]), &anynull, &fits_status);
    if (fits_status) {
      fits_report_error(stderr, fits_status);
      return AS_QUIT_ERROR;
    }
    std::cout << "  ** -> OK "<< std::endl;
  }
  
  fits_close_file(fits, &fits_status);
  if (fits_status) {
    fits_report_error(stderr, fits_status);
    return AS_QUIT_ERROR;
  }

  return AS_OK;
}

ANLStatus AHRayTracingPrimaryGen::mod_analyze()
{
  if (m_ID == m_EventNum) {
    return AS_QUIT;
  }
    
  return BasicPrimaryGen::mod_analyze();
}

void AHRayTracingPrimaryGen::makePrimarySetting()
{
  const int id = m_ID;
  double energy = m_Columns[0][id]*unit::keV;
  const double x = m_Columns[1][id]*unit::mm;
  const double y = m_Columns[2][id]*unit::mm;
  const double z = 0.0;
  const double xDirection = m_Columns[3][id];
  const double yDirection = m_Columns[4][id];
  const double zDirection = m_Columns[5][id];
  const G4ThreeVector position = m_offset + G4ThreeVector(x, y, z);
  const G4ThreeVector direction(xDirection, yDirection, zDirection);
  if (m_EnergyResample) {
    energy = sampleEnergy();
  }
  
  setPrimary(position, energy, direction.unit());
  ++m_ID;
}

} /* namespace comptonsoft */
