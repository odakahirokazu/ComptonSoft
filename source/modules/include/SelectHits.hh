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

#ifndef COMPTONSOFT_SelectHits_H
#define COMPTONSOFT_SelectHits_H 1

#include "VCSModule.hh"

#include <map>
#include <tuple>
#include "CSHitCollection.hh"

namespace comptonsoft {

class SelectHits : public VCSModule
{
  DEFINE_ANL_MODULE(SelectHits, 2.2);
public:
  SelectHits();
  ~SelectHits();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();

protected:
  void insertHitIntoTheCollection(const DetectorHit_sptr& hit)
  { m_HitCollection->insertHit(hit); }
  
private:
  virtual bool setAnalysisParameters();
  virtual void doProcessing();
  virtual void collectHits();

  virtual bool setThresholdEnergy(VRealDetectorUnit* detector,
                                  double threshold,
                                  double thresholdCathode,
                                  double thresholdAnode);

  bool setEnergyConsistencyCheckFunctions(VRealDetectorUnit* detector,
                                          double lowerC0,
                                          double lowerC1,
                                          double upperC0,
                                          double upperC1);

private:
  CSHitCollection* m_HitCollection = nullptr;

  int m_DetectorType;
  int m_ReconstructionMode;
  double m_Threshold;
  double m_ThresholdCathode;
  double m_ThresholdAnode;
  double m_LowerECheckFuncC0;
  double m_LowerECheckFuncC1;
  double m_UpperECheckFuncC0;
  double m_UpperECheckFuncC1;
  std::map<std::string,
           std::tuple<int, int,
                      double, double, double,
                      double, double, double, double>> m_AnalysisMap;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SelectHits_H */
