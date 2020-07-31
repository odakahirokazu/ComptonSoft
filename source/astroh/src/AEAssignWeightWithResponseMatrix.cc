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

#include "AEAssignWeightWithResponseMatrix.hh"

#include "TH2.h"
#include "AstroUnits.hh"
#include "InitialInformation.hh"
#include "DetectorHit.hh"
#include "CSHitCollection.hh"
#include "TFile.h"

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

AEAssignWeightWithResponseMatrix::AEAssignWeightWithResponseMatrix()
{
}

AEAssignWeightWithResponseMatrix::~AEAssignWeightWithResponseMatrix() = default;

ANLStatus AEAssignWeightWithResponseMatrix::mod_define()
{
  define_parameter("root_filename", &mod_class::rootFilename_);
  define_parameter("fits_filename", &mod_class::fitsFilename_);
  define_parameter("histogram_name", &mod_class::histName_);
  
  return AS_OK;
}

ANLStatus AEAssignWeightWithResponseMatrix::mod_initialize()
{
  get_module_NC("CSHitCollection", &hitCollection_);
  get_module_IFNC("InitialInformation", &initialInfo_);

  ANLStatus status = AS_OK;
  
  readRootResponse();
  normalizeResponseHistogram(response_);
  
  initializeHistogram();
  status = readFitsResponse();
  if (status!=AS_OK) {
    return status;
  }
  calculateUndetectedProbability();
  fillRefResponseHistogram();
  normalizeResponseHistogram(refResponse_);
  calculateWeight();

  return AS_OK;
}

ANLStatus AEAssignWeightWithResponseMatrix::mod_analyze()
{
  const int timeGroup = 0;
  std::vector<DetectorHit_sptr>& hits = hitCollection_->getHits(timeGroup);
  
  const double ini_energy = initialInfo_->InitialEnergy();
  double energy = 0.0;
  if (hits.size()>0) {
    energy = hits[0]->Energy();
  }
  
  const int ix = weightHist_->GetXaxis()->FindBin(ini_energy/unit::keV);
  const int iy = weightHist_->GetYaxis()->FindBin(energy/unit::keV);
  const double w = weightHist_->GetBinContent(ix, iy);
  initialInfo_->setWeight(w);
  
  return AS_OK;
}

void AEAssignWeightWithResponseMatrix::readRootResponse()
{
  TFile* f = new TFile(rootFilename_.c_str());
  response_ = (TH2F*) f->Get(histName_.c_str());
}

void AEAssignWeightWithResponseMatrix::initializeHistogram()
{
  const std::string name1 = "response matrix";
  const std::string title1 = "response matrix";
  const std::string name2 = "weight";
  const std::string title2 = "weight";
  const int nx = response_->GetNbinsX();
  const int ny = response_->GetNbinsY();
  const double xmin = response_->GetXaxis()->GetXmin();
  const double xmax = response_->GetXaxis()->GetXmax();
  const double ymin = response_->GetYaxis()->GetXmin();
  const double ymax = response_->GetYaxis()->GetXmax();

  refResponse_ = new TH2F(name1.c_str(), title1.c_str(),
                          nx, xmin, xmax,
                          ny, ymin, ymax);
  weightHist_ = new TH2F(name2.c_str(), title2.c_str(),
                          nx, xmin, xmax,
                          ny, ymin, ymax);
}

ANLStatus AEAssignWeightWithResponseMatrix::readFitsResponse()
{
  cfitsio::fitsfile* fitsFile = nullptr;
  int fitsStatus = 0;
  int nfound = 0;
  long matrix_naxes[2] = {1, 1};
  long ebounds_naxes[2] = {1, 1};
  const std::vector<std::string> matrix_colname = {"ENERG_LO", "ENERG_HI", "F_CHAN", "MATRIX"};
  const std::vector<std::string> ebounds_colname = {"E_MIN", "E_MAX"};
  
  cfitsio::fits_open_file(&fitsFile, fitsFilename_.c_str(), READONLY, &fitsStatus);
  if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      return AS_QUIT_ERROR;
  }
  cfitsio::fits_movabs_hdu(fitsFile, 2, IMAGE_HDU, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_QUIT_ERROR;
  }
  cfitsio::fits_read_keys_lng(fitsFile, (char*)"NAXIS", 1, 2, matrix_naxes, &nfound, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_QUIT_ERROR;
  }
  const int num_energy_bin = matrix_naxes[1];
  refResponseArray_.resize(num_energy_bin);
  energyRange_.resize(num_energy_bin);
  firstChannel_.resize(num_energy_bin);

  for (int i=0; i<static_cast<int>(matrix_colname.size()); i++) {
    int colid;
    std::vector<double> buf(num_energy_bin);
    const long first_row = 1;
    const long first_element = 1;
    double nulval = 0.0;
    int anynul = 0;
    cfitsio::fits_get_colnum(fitsFile, CASEINSEN, const_cast<char*>(matrix_colname[i].c_str()), &colid, &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      return AS_QUIT_ERROR;
    }
    if (i<=2) {
      cfitsio::fits_read_col(fitsFile, TDOUBLE, colid, first_row, first_element, num_energy_bin,
                             &nulval, &buf[0], &anynul, &fitsStatus);
      if (fitsStatus) {
        cfitsio::fits_report_error(stderr, fitsStatus);
        return AS_QUIT_ERROR;
      }
      for (int j=0; j<num_energy_bin; j++) {
        if (i==0) energyRange_[j].first = buf[j];
        if (i==1) energyRange_[j].second = buf[j];
        if (i==2) firstChannel_[j] = buf[j];
      }
    }
    if (i==3) {
      for (int j=0; j<num_energy_bin; j++) {
        long row_num = j+1;
        long length = 0;
        long offset = 0;
        cfitsio::fits_read_descript(fitsFile, colid, row_num, &length, &offset, &fitsStatus);
        if (fitsStatus) {
          cfitsio::fits_report_error(stderr, fitsStatus);
          return AS_QUIT_ERROR;
        }
        refResponseArray_[j].resize(length);
        cfitsio::fits_read_col(fitsFile, TDOUBLE, colid, row_num, first_element, length,
                          &nulval, &refResponseArray_[j][0], &anynul, &fitsStatus);
        if (fitsStatus) {
          cfitsio::fits_report_error(stderr, fitsStatus);
          return AS_QUIT_ERROR;
        }
      }
    }
  }

  cfitsio::fits_movabs_hdu(fitsFile, 3, IMAGE_HDU, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_QUIT_ERROR;
  }
  cfitsio::fits_read_keys_lng(fitsFile, (char*)"NAXIS", 1, 2, ebounds_naxes, &nfound, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_QUIT_ERROR;
  }
  const int num_pulse_bin = ebounds_naxes[1];
  pulseRange_.resize(num_pulse_bin);

  for (int i=0; i<static_cast<int>(ebounds_colname.size()); i++) {
    int colid;
    std::vector<double> buf(num_pulse_bin);
    const long first_row = 1;
    const long first_element = 1;
    double nulval = 0.0;
    int anynul = 0;
    cfitsio::fits_get_colnum(fitsFile, CASEINSEN, const_cast<char*>(ebounds_colname[i].c_str()), &colid, &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      return AS_QUIT_ERROR;
    }
    cfitsio::fits_read_col(fitsFile, TDOUBLE, colid, first_row, first_element, num_pulse_bin,
                           &nulval, &buf[0], &anynul, &fitsStatus);
    if (fitsStatus) {
      cfitsio::fits_report_error(stderr, fitsStatus);
      return AS_QUIT_ERROR;
    }
    for (int j=0; j<num_pulse_bin; j++) {
      if (i==0) pulseRange_[j].first = buf[j];
      if (i==1) pulseRange_[j].second = buf[j];
    }
  }
  
  cfitsio::fits_close_file(fitsFile, &fitsStatus);
  if (fitsStatus) {
    cfitsio::fits_report_error(stderr, fitsStatus);
    return AS_QUIT_ERROR;
  }
  
  return AS_OK;
}

void AEAssignWeightWithResponseMatrix::calculateUndetectedProbability()
{
  const int n = refResponseArray_.size();
  undetectedProb_.resize(n);
  for (int i=0; i<n; i++) {
    double sum = 0.0;
    for (double p: refResponseArray_[i]) {
      sum += p;
    }
    undetectedProb_[i] = 1.0 - sum;
  }
}

void AEAssignWeightWithResponseMatrix::fillRefResponseHistogram()
{
  const int n = refResponseArray_.size();
  const int num_div = 10;
  
  for (int i=0; i<n; i++) {
    const int length = refResponseArray_[i].size();
    for (int j=0; j<length; j++) {
      for (int dx=0; dx<num_div; dx++) {
        for (int dy=0; dy<num_div; dy++) {
          const double ini_energy = energyRange_[i].first + (energyRange_[i].second-energyRange_[i].first)*(0.5+dx)/num_div;
          const int ch = firstChannel_[i] + j;
          const double ph = pulseRange_[ch].first + (pulseRange_[ch].second-pulseRange_[ch].first)*(0.5+dy)/num_div;
          const double w = refResponseArray_[i][j]/(num_div*num_div);
          refResponse_->Fill(ini_energy, ph, w);
        }
      }
    }
  }
  for (int i=0; i<n; i++) {
    for (int dx=0; dx<num_div; dx++) {
      const double ini_energy = energyRange_[i].first + (energyRange_[i].second-energyRange_[i].first)*(0.5+dx)/num_div;
      const double ph = 0.0;
      const double w = undetectedProb_[i]/num_div;
      refResponse_->Fill(ini_energy, ph, w);
    }
  }
}

void AEAssignWeightWithResponseMatrix::calculateWeight()
{
  const double eps = 1E-9;
  const int nx = weightHist_->GetNbinsX();
  const int ny = weightHist_->GetNbinsY();
  for (int ix=1; ix<=nx; ix++) {
    for (int iy=1; iy<=ny; iy++) {
      const double v = response_->GetBinContent(ix, iy);
      const double v_ref = refResponse_->GetBinContent(ix, iy);
      double w;
      if (v<eps || v_ref<eps) {
        w = 1.0;
      }
      else {
        w = v_ref/v;
      }
      weightHist_->SetBinContent(ix, iy, w);
    }
  }
}

void AEAssignWeightWithResponseMatrix::normalizeResponseHistogram(TH2* resp)
{
  const int nx = resp->GetNbinsX();
  const int ny = resp->GetNbinsY();
  for (int ix=1; ix<=nx; ix++) {
    double sum = 0.0;
    for (int iy=1; iy<=ny; iy++) {
      sum += resp->GetBinContent(ix, iy);
    }
    if (sum==0.0) continue;
    for (int iy=1; iy<=ny; iy++) {
      const double v = resp->GetBinContent(ix, iy);
      resp->SetBinContent(ix, iy, v/sum);
    }
  }
}

} /* namespace comptonsoft */
