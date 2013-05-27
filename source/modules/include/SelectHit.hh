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

#ifndef COMPTONSOFT_SelectHit_H
#define COMPTONSOFT_SelectHit_H 1

#include "VCSModule.hh"

#include <map>
#include <boost/tuple/tuple.hpp>
#include "CSHitCollection.hh"

namespace comptonsoft {


class SelectHit : public VCSModule
{
  DEFINE_ANL_MODULE(SelectHit, 2.1);
public:
  SelectHit();
  ~SelectHit() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_ana();

protected:
  void InsertHitIntoTheCollection(comptonsoft::DetectorHit_sptr hit)
  { hit_collection->InsertHit(hit); }
  
  std::map<std::string,
           boost::tuple<int, int, double, double, double> > m_AnalysisMap;  

private:
  virtual bool setAnalysisParam();
  virtual void doProcessing();
  virtual void collectHits();

  CSHitCollection* hit_collection;

  int m_DetectorType;
  int m_AnalysisMode;
  double m_Threshold;
  double m_ThresholdCathode;
  double m_ThresholdAnode;
};

}

#endif /* COMPTONSOFT_SelectHit_H */
