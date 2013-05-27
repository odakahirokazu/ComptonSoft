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

#ifndef COMPTONSOFT_HitPattern_H
#define COMPTONSOFT_HitPattern_H 1

#include <string>
#include <vector>

#include "DetectorHit_sptr.hh"
#include "DetectorGroup.hh"

namespace comptonsoft {

/**
 * A class of a hit pattern.
 * A hit pattern is defined as a list of a detector group.
 * @author Hirokazu Odaka
 * @date 2008-10-22
 */

class HitPattern
{
public:
  const std::string& Name() const { return name; }
  const std::string& ShortName() const { return short_name; }
  void setName(const std::string& str) { name = str; }
  void setShortName(const std::string& str) { short_name = str; }
  
  /**
   * add a detector group to this hit pattern.
   * @param dg a detector group
  */
  void add(const DetectorGroup& dg) { detgroup.push_back(dg); }

  /**
   * check if the given hit vector matches this hit pattern.
   * @param hitvec a hit vector to check
   */
  bool match(const std::vector<DetectorHit_sptr>& hitvec) const;

  /**
   * check if the given hit vector matches this hit pattern.
   * @param hitvec a hit vector to check
  */
  bool match(const std::vector<DetectorHit*>& hitvec) const;

  /**
   * check if the given hit vector matches this hit pattern.
   * @param idvec a vector of the detector IDs of a hit vector to check
  */
  bool match(const std::vector<int>& idvec) const;

  /**
   * @return a number of hits to define this hit pattern.
   */
  int NumberOfHits() const { return detgroup.size(); }

private:
  std::string name;
  std::string short_name;
 
  std::vector<DetectorGroup> detgroup;
};

}

#endif /* COMPTONSOFT_HitPattern_H */
