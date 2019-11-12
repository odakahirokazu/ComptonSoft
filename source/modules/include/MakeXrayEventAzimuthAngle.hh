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
 * MakeXrayEventAzimuthAngle
 *
 * @author Tsubasa Tamba
 * @date 2019-11-13
 */

#ifndef COMPTONSOFT_MakeXrayEventAzimuthAngle_H
#define COMPTONSOFT_MakeXrayEventAzimuthAngle_H 1

#include <anlnext/BasicModule.hh>
#include "XrayEvent.hh"
#include "VCSModule.hh"
class TH1;

namespace comptonsoft {

class XrayEventCollection;
class XrayEventTreeIO;

class MakeXrayEventAzimuthAngle : public VCSModule
{
  DEFINE_ANL_MODULE(MakeXrayEventAzimuthAngle, 1.0);
  // ENABLE_PARALLEL_RUN();
public:
  MakeXrayEventAzimuthAngle();
  
protected:
  MakeXrayEventAzimuthAngle(const MakeXrayEventAzimuthAngle&);

public:
  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  std::vector<double>& Distribution() { return distribution_; }
  std::vector<double>& TotalDistribution() { return totalDistribution_; }
  double AngleMin() { return angleMin_; }
  double AngleMax() { return angleMax_; }
  int NumBin() { return numBin_; }

  void drawOutputFiles(TCanvas* c1, std::vector<std::string>* filenames) override;

protected:
  void resetDistribution(std::vector<double>& distribution) const;
  void fillHistogram();

private:
  std::string collectionModule_;

  std::vector<double> distribution_;
  std::vector<double> totalDistribution_;
  double angleMin_ = -180.0-0.5*11.25;
  double angleMax_ = 180.0+0.5*11.25;
  int numBin_ = 33;
  
  XrayEventCollection* collection_ = nullptr;
  TH1* totalHistogram_ = nullptr;
  std::string outputFile_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_MakeXrayEventAzimuthAngle_H */
