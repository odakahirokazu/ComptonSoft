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

/**
 * MakeXrayEventSpectrum
 *
 * @author Tsubasa Tamba
 * @date 2019-11-12
 */

#ifndef COMPTONSOFT_MakeXrayEventSpectrum_H
#define COMPTONSOFT_MakeXrayEventSpectrum_H 1

#include <anlnext/BasicModule.hh>
#include "XrayEvent.hh"
#include "VCSModule.hh"
class TH1;

namespace comptonsoft {

class XrayEventCollection;
class XrayEventTreeIO;

class MakeXrayEventSpectrum : public VCSModule
{
  DEFINE_ANL_MODULE(MakeXrayEventSpectrum, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  MakeXrayEventSpectrum();
  
protected:
  MakeXrayEventSpectrum(const MakeXrayEventSpectrum&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  std::vector<double>& Spectrum() { return spectrum_; }
  std::vector<double>& TotalSpectrum() { return totalSpectrum_; }
  double EnergyMin() { return energyMin_; }
  double EnergyMax() { return energyMax_; }
  int NumBin() { return numBin_; }

  void drawOutputFiles(TCanvas* c1, std::vector<std::string>* filenames) override;

protected:
  void resetSpectrum(std::vector<double>& spectrum) const;
  void fillHistogram();

private:
  std::string collectionModule_;

  std::vector<double> spectrum_;
  std::vector<double> totalSpectrum_;
  double energyMin_ = 0.0;
  double energyMax_ = 10000.0;
  int numBin_ = 1000;
  
  XrayEventCollection* collection_ = nullptr;
  TH1* totalHistogram_ = nullptr;
  std::string outputFile_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_MakeXrayEventSpectrum_H */
