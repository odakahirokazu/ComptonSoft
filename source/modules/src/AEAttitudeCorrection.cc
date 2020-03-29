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

#include "AEAttitudeCorrection.hh"

#include <random>
#include "Randomize.hh"

namespace cfitsio
{
extern "C" {
#include "fitsio.h"
}
}

using namespace anlnext;

namespace comptonsoft
{

AEAttitudeCorrection::AEAttitudeCorrection()
{
}

AEAttitudeCorrection::~AEAttitudeCorrection() = default;

ANLStatus AEAttitudeCorrection::mod_define()
{
  define_parameter("filename", &mod_class::filename_);
  define_parameter("pixel_center_x", &mod_class::pixelCenterX_);
  define_parameter("pixel_center_y", &mod_class::pixelCenterY_);
  define_parameter("time_bin", &mod_class::timeBin_);
  define_parameter("make_log", &mod_class::makeLog_);
  define_parameter("log_filename", &mod_class::logFilename_);
  define_parameter("num_event_proportion_limit", &mod_class::numEventProportionLimit_);

  return AS_OK;
}

ANLStatus AEAttitudeCorrection::mod_initialize()
{
  int fitsStatus = 0;
  std::string colname[numColumns_] = {"X", "Y", "TIME"};
  long naxes[2] = {0, 0};
  int nfound = 0;
  int anynul = 0;
  double doublenul = 0.0;
  short shortnul = 0;

  cfitsio::fits_open_file(&fitsFile, filename_.c_str(), READWRITE, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::ffmahd(fitsFile, 2, IMAGE_HDU, &fitsStatus);
  if (fitsStatus){
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  for (int i=0; i<numColumns_; i++) {
    cfitsio::fits_get_colnum(fitsFile, CASEINSEN, const_cast<char*>(colname[i].c_str()), &colid_[i], &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      return AS_ERROR;
    }
  }

  cfitsio::fits_read_keys_lng(fitsFile, (char*)"NAXIS", 1, 2, naxes, &nfound, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }
  numEvent_ = static_cast<int>(naxes[1]);

  pixelArrayX_.resize(numEvent_);
  pixelArrayY_.resize(numEvent_);
  eventTime_.resize(numEvent_);

  cfitsio::fits_read_col(fitsFile, TSHORT, colid_[0], (long)1, (long)1, naxes[1], &shortnul,
                         &pixelArrayX_[0], &anynul, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::fits_read_col(fitsFile, TSHORT, colid_[1], (long)1, (long)1, naxes[1], &shortnul,
                         &pixelArrayY_[0], &anynul, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::fits_read_col(fitsFile, TDOUBLE, colid_[2], (long)1, (long)1, naxes[1], &doublenul,
                         &eventTime_[0], &anynul, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  startTime_ = eventTime_[0];
  std::cout << "Number of Event: " << numEvent_ << std::endl;

  if (makeLog_) {
    logFile_ = std::ofstream(logFilename_.c_str());
  }

  return AS_OK;
}

ANLStatus AEAttitudeCorrection::mod_analyze()
{  
  const int n = get_loop_index();
  if (n==numEvent_) {
    return AS_QUIT;
  }

  const int x = pixelArrayX_[n];
  const int y = pixelArrayY_[n];
  const double t = eventTime_[n];

  if (t-startTime_>timeBin_ || n==numEvent_-1) {
    if (n-startID_ > numEventLowerBound_) {
      currentPixelCenterX_ = sumx_/(n-startID_);
      currentPixelCenterY_ = sumy_/(n-startID_);
      numEventLowerBound_ = numEventProportionLimit_ * (n-startID_);
    }
    if (makeLog_) {
      logFile_ << n-startID_ << " "  << currentPixelCenterX_ << " " << currentPixelCenterY_ << "\n";
    }
    for (int i=startID_; i<n; i++) {
      const double dix = G4UniformRand() - 0.5;
      const double diy = G4UniformRand() - 0.5;
      pixelArrayX_[i] = static_cast<int>(pixelArrayX_[i] + dix - currentPixelCenterX_ + pixelCenterX_);
      pixelArrayY_[i] = static_cast<int>(pixelArrayY_[i] + diy - currentPixelCenterY_ + pixelCenterY_);
      if (pixelArrayX_[i]<=0) { pixelArrayX_[i] = 1; }
      if (pixelArrayY_[i]<=0) { pixelArrayY_[i] = 1; }
    }
    sumx_ = 0.0;
    sumy_ = 0.0;
    startID_ = n;
    while (t-startTime_>timeBin_) {
      startTime_ += timeBin_;
    }
  }

  sumx_ += x;
  sumy_ += y;

  return AS_OK;
}

ANLStatus AEAttitudeCorrection::mod_end_run()
{
  int fitsStatus = 0;

  cfitsio::fits_write_col(fitsFile, TSHORT, colid_[0], (long)1, (long)1, numEvent_,
                          &pixelArrayX_[0], &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::fits_write_col(fitsFile, TSHORT, colid_[1], (long)1, (long)1, numEvent_,
                          &pixelArrayY_[0], &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_ERROR;
  }

  if (makeLog_) {
    logFile_.close();
  }

  return AS_OK;
}



} /* namespace comptonsoft */
