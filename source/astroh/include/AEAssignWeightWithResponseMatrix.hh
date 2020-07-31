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

#ifndef COMPTONSOFT_AEAssignWeightWithResponseMatrix_H
#define COMPTONSOFT_AEAssignWeightWithResponseMatrix_H 1

#include "VCSModule.hh"
#include "CSHitCollection.hh"

class TH2;
namespace anlgeant4 { class InitialInformation; }

namespace comptonsoft {


/**
 * @author Tsubasa Tamba
 * @date 2020-07-27
 */
class AEAssignWeightWithResponseMatrix : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(AEAssignWeightWithResponseMatrix, 2.0);
public:
  AEAssignWeightWithResponseMatrix();
  ~AEAssignWeightWithResponseMatrix();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

protected:
  void readRootResponse();
  void initializeHistogram();
  anlnext::ANLStatus readFitsResponse();
  void calculateUndetectedProbability();
  void fillRefResponseHistogram();
  void calculateWeight();
  void normalizeResponseHistogram(TH2* resp);
  
private:
  std::string rootFilename_;
  std::string fitsFilename_;
  std::string histName_;
  
  CSHitCollection* hitCollection_ = nullptr;
  anlgeant4::InitialInformation* initialInfo_;
  TH2* response_;
  TH2* refResponse_;
  TH2* weightHist_;
  std::vector<std::vector<double>> refResponseArray_;
  std::vector<std::pair<double, double>> energyRange_;
  std::vector<double> firstChannel_;
  std::vector<std::pair<double, double>> pulseRange_;
  std::vector<double> undetectedProb_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AEAssignWeightWithResponseMatrix_H */
