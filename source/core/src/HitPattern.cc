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
#include <algorithm>
#include "DetectorHit.hh"

namespace comptonsoft {

bool HitPattern::match(const std::vector<DetectorHit_sptr>& hits) const
{
  std::vector<int> ids(hits.size());
  std::transform(std::begin(hits), std::end(hits), std::begin(ids),
                 [](DetectorHit_sptr hit){
                   return hit->DetectorID();
                 });
  return match(ids);
}

bool HitPattern::match(const std::vector<int>& ids) const
{
  const std::size_t NHits = ids.size();
  const std::size_t NGroups = groups_.size();

  if (NHits != NGroups) { return false; }
  
  std::vector<int> matchFlags(NGroups, 0);
  for (std::size_t i=0; i<NHits; i++) {
    bool match1 = false;
    const int detid = ids[i];
    for (std::size_t iGroup=0; iGroup<NGroups; iGroup++) {
      if (matchFlags[iGroup] == 0 && groups_[iGroup].isMember(detid)) {
        matchFlags[iGroup] = 1;
        match1 = true;
        break;
      }
    }
    if (!match1) return false;
  }
  return true;
}

} /* namespace comptonsoft */
