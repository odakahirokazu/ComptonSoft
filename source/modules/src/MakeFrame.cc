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

#include "MakeFrame.hh"

#include <random>
#include <array>
#include <string>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include "AstroUnits.hh"
#include "CSHitCollection.hh"
#include "DetectorHit.hh"


namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft
{

MakeFrame::MakeFrame()
{
}

MakeFrame::~MakeFrame() = default;

ANLStatus MakeFrame::mod_define()
{
  define_parameter("filename_base", &mod_class::m_FileNameBase);
  set_parameter_description ("The output filename will be \"(filename_base)_FrameN.fits\".");
  define_parameter("num_pixelx", &mod_class::m_NumPixelX);
  set_parameter_description ("Number of pixels on x-axis.");
  define_parameter("num_pixely", &mod_class::m_NumPixelY);
  set_parameter_description("Numbere of pixels on y-axis.");
  return AS_OK;
}

ANLStatus MakeFrame::mod_initialize()
{
  VCSModule::mod_initialize();
  get_module_NC("CSHitCollection", &m_HitCollection);

  m_NumPixelsArray[0] = m_NumPixelX;
  m_NumPixelsArray[1] = m_NumPixelY;
  m_NumPixels = m_NumPixelX * m_NumPixelY;
  m_EnergyArray.resize(m_NumPixels, 0.0);

  return AS_OK;
}

ANLStatus MakeFrame::mod_analyze()
{
  m_EnergyArray.assign(m_NumPixels, 0.0);

  const std::vector<DetectorHit_sptr>& firstHits = m_HitCollection->getHits(0);
  if (firstHits.size() == 0) { return AS_OK; }

  const DetectorHit_sptr& hit0 = firstHits[0];
  const long frameID = hit0->EventID();
  const std::string filename = m_FileNameBase + "_Frame" + std::to_string(frameID) + ".fits";

  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  cfitsio::fits_create_file(&fitsFile, filename.c_str(), &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::fits_create_img(fitsFile, DOUBLE_IMG, 2, m_NumPixelsArray, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
  for (int timeGroup=0; timeGroup<NumTimeGroups; timeGroup++) {
    const std::vector<DetectorHit_sptr>& hits = m_HitCollection->getHits(timeGroup);
    for (DetectorHit_sptr hit: hits) {
      if (hit->PixelX() >= m_NumPixelX || hit->PixelY() >= m_NumPixelY){
        std::cout << "This hit occurs outside the image area." << std::endl;
        return AS_ERROR;
      }
      const int pixelIndex = hit->PixelY() * m_NumPixelsArray[0] + hit->PixelX();
      m_EnergyArray[pixelIndex] += hit->Energy();

#if 0 // DEBUG
      std::cout << "Pixel index: " << pixelIndex << std::endl;
      std::cout << "a[]: " << m_EnergyArray[pixelIndex] << std::endl;
      const double a = hit -> Energy();
      const double b = hit -> PositionX();
      const vector3_t c = hit-> Position();
      const int d = hit -> PixelX();
      const int e = hit -> PixelY();
      std::cout << "ENERGY " << a << std::endl;
      std::cout << "PositionX " << b << std::endl;
      std::cout << "Position " << c << std::endl;
      std::cout << "PixelX " << d << std::endl;
      std::cout << "PixelY " << e << std::endl;
#endif
    }

#if 0
    std::cout << frameID << std::endl;
#endif
  }
  
  long StartPixel[2] = {1, 1};
  cfitsio::fits_write_pix(fitsFile, TDOUBLE, StartPixel, m_NumPixels, &m_EnergyArray[0], &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }
  
  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  return AS_OK;
}

} /* namespace comptonsoft */
