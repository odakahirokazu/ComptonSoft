/*************************************************************************
 *                                                                       *
 * Copyright (c) 2019 Hirokazu Odaka                                     *
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

#include "WritePedestals.hh"
#include "FrameData.hh"
#include <string>

namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

using namespace anlnext;

namespace comptonsoft
{

void write_image_to_fits(const image_t& image,
                         cfitsio::fitsfile* fitsFile,
                         int* fitsStatus)
{
  const int nx = image.shape()[0];
  const int ny = image.shape()[1];

  std::vector<double> array;
  array.reserve(nx*ny);
  for (int iy=0; iy<ny; iy++) {
    for (int ix=0; ix<nx; ix++) {
      array.push_back(image[ix][iy]);
    }
  }

  long NumPixelsArray[2] = {nx, ny};
  const long NumPixels = nx*ny;
  long StartPixel[2] = {1, 1};
  
  cfitsio::fits_create_img(fitsFile, DOUBLE_IMG, 2, NumPixelsArray, fitsStatus);
  if (*fitsStatus) { return; }

  cfitsio::fits_write_pix(fitsFile, TDOUBLE, StartPixel, NumPixels, &array[0], fitsStatus);
}

WritePedestals::WritePedestals()
  : filename_("pedestals.fits")
{
}

ANLStatus WritePedestals::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  define_parameter("detector_id", &mod_class::detector_id_);
  
  return AS_OK;
}

ANLStatus WritePedestals::mod_end_run()
{
  VRealDetectorUnit* detector = getDetectorManager()->getDetectorByID(detector_id_);
  if (detector == nullptr) {
    std::cout << "Detector " << detector_id_ << " does not exist." << std::endl;
    return AS_OK;
  }
  
  const FrameData* frame = detector->getFrameData();
  const image_t& pedestals = frame->getPedestals();
  const image_t& frameDeviation = frame->getDeviationFrame();

  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  
  cfitsio::fits_create_file(&fitsFile, filename_.c_str(), &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  write_image_to_fits(pedestals, fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  write_image_to_fits(frameDeviation, fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }
  cfitsio::fits_write_key(fitsFile, TSTRING, "EXTNAME", (char*)"Sigma", "Standard deviation of the pedestal values", &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error (stderr, fitsStatus);
    return AS_ERROR;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
