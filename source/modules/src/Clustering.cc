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

#include "Clustering.hh"

#include <fstream>
#include "TMath.h"
#include "globals.hh"

#include "DetectorHit.hh"

using namespace comptonsoft;
using namespace anl;


Clustering::Clustering()
  : hit_collection(0)
{
}


ANLStatus Clustering::mod_init()
{
  GetANLModuleNC("CSHitCollection", &hit_collection);
  EvsDef("Clustering::Clustered");

  return AS_OK;
}


ANLStatus Clustering::mod_ana()
{
  int numTimeGroup = hit_collection->NumTimeGroup();
  int timeGroup = numTimeGroup - 1;
  std::vector<DetectorHit_sptr>& hitVec
    = hit_collection->GetHits(timeGroup);

  std::vector<DetectorHit_sptr>::iterator it1;
  std::vector<DetectorHit_sptr>::iterator it2;
  
  for (it1 = hitVec.begin(); it1 != hitVec.end(); ++it1) {
    it2 = it1;
    ++it2;

    while ( it2 != hitVec.end() ) {
      if ( (*it2)->areAdjacent(**it1) ) {
#if 0
        std::cout << "Clustering: \n"
                  << "ID: " << (*it1)->getDetID() << " " << (*it2)->getDetID() << "\n"
                  << "Strip X: " << (*it1)->getStripX() << " " << (*it2)->getStripX() << "\n"
                  << "Strip Y: " << (*it1)->getStripY() << " " << (*it2)->getStripY() << std::endl;
#endif
        **it1 += **it2;
        it2 = hitVec.erase(it2);
        EvsSet("Clustering::Clustered");
      }
      else {
        ++it2;
      }
    }
  }

  return AS_OK;
}

