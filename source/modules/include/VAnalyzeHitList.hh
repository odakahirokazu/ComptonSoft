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

#ifndef COMPTONSOFT_VAnalyzeHitList_H
#define COMPTONSOFT_VAnalyzeHitList_H 1

#include "VCSModule.hh"


namespace comptonsoft {

class CSHitCollection;

/**
 * Virtual class for analyze a hit list and making histograms.
 * 
 * @author Hirokazu Odaka
 * @date 2012-06-20
 */
class VAnalyzeHitList : public VCSModule
{
  DEFINE_ANL_MODULE(VAnalyzeHitList, 1.0);
public:
  VAnalyzeHitList();
  ~VAnalyzeHitList() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();
  anl::ANLStatus mod_endrun();

private:
  virtual void define_histograms() {}
  virtual void fill_histograms(const std::vector<DetectorHit_sptr>& ) {}
  virtual void finish_histograms() {}
  
private:
  CSHitCollection* hit_collection;
  std::string directory_name;
};

}

#endif /* COMPTONSOFT_VAnalyzeHitList_H */
