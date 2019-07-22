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

#include "SetPedestals.hh"
#include "FrameData.hh"
#include "ConstructFrame.hh"

namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

using namespace anlnext;

namespace comptonsoft{

SetPedestals::SetPedestals()
  : filename_("pedestals.fits")
{
}

ANLStatus SetPedestals::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  return AS_OK;
}

ANLStatus SetPedestals::mod_initialize()
{
  get_module_NC("ConstructFrame", &frame_owner_);
  return AS_OK;
}

ANLStatus SetPedestals::mod_begin_run()
{
  FrameData& frameData = frame_owner_->getFrame();

  image_t& pedestals = frameData.getPedestals();
  const int nx = pedestals.shape()[0];
  const int ny = pedestals.shape()[1];

  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  long StartPixel[2] = {1, 1};
  const long NumPixels = nx*ny;
  int nulval = 0;
  int anynul = 0;

  std::vector<double> array;
  array.resize(NumPixels, 0.0);

  cfitsio::fits_open_image(&fitsFile, filename_.c_str(), READONLY, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::fits_read_pix(fitsFile, TDOUBLE, StartPixel, NumPixels, &nulval, &array[0], &anynul, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  for (int iy=0; iy<ny; iy++){
    for (int ix=0; ix<nx; ix++){
      pedestals[ix][iy] = array[iy*nx+ix];
    }
  }

  return AS_OK;
}

} /* namespace comptonsoft */
