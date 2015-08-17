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

#ifndef COMPTONSOFT_MakeDetectorHitsTR_H
#define COMPTONSOFT_MakeDetectorHitsTR_H 1

#include <vector>

#include "MakeDetectorHits.hh"

namespace comptonsoft {

class MakeDetectorHitsTR : public MakeDetectorHits
{
  DEFINE_ANL_MODULE(MakeDetectorHitsTR, 2.1);
public:
  MakeDetectorHitsTR() = default;
  ~MakeDetectorHitsTR() = default;
  
private:
  void doProcessing();

private:
  std::vector<double> m_TriggerTimes;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_MakeDetectorHitsTR_H */
