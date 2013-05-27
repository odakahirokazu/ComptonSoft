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

// 2011-02-09 Hirokazu Odaka

#ifndef COMPTONSOFT_Clustering_H
#define COMPTONSOFT_Clustering_H 1

#include <vector>
#include "BasicModule.hh"
#include "DetectorGroup.hh"
#include "HitPattern.hh"
#include "CSHitCollection.hh"
#include "ComptonEvent.hh"

namespace comptonsoft {

class Clustering : public anl::BasicModule
{
  DEFINE_ANL_MODULE(Clustering, 1.0)
public:
  Clustering();
  ~Clustering() {}

  virtual anl::ANLStatus mod_init();
  virtual anl::ANLStatus mod_ana();

private:
  CSHitCollection* hit_collection;
};

}

#endif /* COMPTONSOFT_Clustering_H */
