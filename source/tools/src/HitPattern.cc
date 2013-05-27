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

#include "HitPattern.hh"
#include "DetectorHit.hh"

namespace comptonsoft {

bool HitPattern::match(const std::vector<DetectorHit*>& hitvec) const
{
  const int numHit = hitvec.size();
  std::vector<int> idvec(numHit);
  for (int i=0; i<numHit; i++) {
    idvec[i] = hitvec[i]->getDetectorID();
  }
  return match(idvec);
}


bool HitPattern::match(const std::vector<DetectorHit_sptr>& hitvec) const
{
  const int numHit = hitvec.size();
  std::vector<int> idvec(numHit);
  for (int i=0; i<numHit; i++) {
    idvec[i] = hitvec[i]->getDetectorID();
  }
  return match(idvec);
}


bool HitPattern::match(const std::vector<int>& idvec) const
{
  const int numHit = idvec.size();

  if ((int)detgroup.size() != numHit) { return false; }

  std::vector<int> match_flag(numHit, 0);

  for (int i=0; i<numHit; i++) {
    bool match1 = false;
    for (int group=0; group<numHit; group++) {
      int detid = idvec[i];
      if (match_flag[group] == 0 &&
	  detgroup[group].isMember(detid)) {
	match_flag[group] = 1;
	match1 = true;
	break;
      }
    }

    if (!match1) return false;
  }

  return true;
}

}
