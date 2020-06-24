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

#ifndef COMPTONSOFT_SelectEventsWithSpectrum_H
#define COMPTONSOFT_SelectEventsWithSpectrum_H 1

#include "VCSModule.hh"
#include <list>
#include "ReadEventTree.hh"

namespace comptonsoft {

class CSHitCollection;


/**
 * @author Tsubasa Tamba
 * @date 2019-12-02
 */
class SelectEventsWithSpectrum : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SelectEventsWithSpectrum, 1.0);
public:
  SelectEventsWithSpectrum();
  ~SelectEventsWithSpectrum();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;

  bool isGoodGrade(int grade);

private:
  double exposure_ = 0.0;
  std::vector<double> energyArray_;
  std::vector<double> countRateArray_;
  std::vector<int> countArray_;
  int numRemainedBin_;
  std::vector<int> photonStack_;
  std::vector<int> goodGrade_;

  CSHitCollection* hitCollection_ = nullptr;
  ReadEventTree* readEventTree_ = nullptr;

};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SelectEventsWithSpectrum_H */
