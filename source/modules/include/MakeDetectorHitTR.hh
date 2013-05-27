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

#ifndef COMPTONSOFT_MakeDetectorHitTR_H
#define COMPTONSOFT_MakeDetectorHitTR_H 1

#include <vector>

#include "MakeDetectorHit.hh"

namespace comptonsoft {

class MakeDetectorHitTR : public MakeDetectorHit
{
  DEFINE_ANL_MODULE(MakeDetectorHitTR, 2.0);
public:
  MakeDetectorHitTR() {}
  ~MakeDetectorHitTR() {}
  
private:
  void doProcessing();

  std::vector<double> trigger_times;
};

}

#endif /* COMPTONSOFT_MakeDetectorHitTR_H */
