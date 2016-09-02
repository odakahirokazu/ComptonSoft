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
  HitPattern() = default;
  ~HitPattern();
  
  const std::string& Name() const { return name_; }
  const std::string& ShortName() const { return shortName_; }
  void setName(const std::string& v) { name_ = v; }
  void setShortName(const std::string& v) { shortName_ = v; }

  unsigned int Bit() const { return bit_; }
  void setBit(unsigned int bit) { bit_ = bit; }
  
  /**
   * add a detector group to this hit pattern.
   * @param dg a detector group
  */
  void add(const DetectorGroup& group) { groups_.push_back(group); }

  /**
   * check if the given hit vector matches this hit pattern.
   * @param hitvec a hit vector to check
   */
  bool match(const std::vector<DetectorHit_sptr>& hits) const;

  /**
   * check if the given hit vector matches this hit pattern.
   * @param idvec a vector of the detector IDs of a hit vector to check
  */
  bool match(const std::vector<int>& ids) const;

  /**
   * @return a number of hits to define this hit pattern.
   */
  int NumberOfHits() const { return groups_.size(); }

private:
  std::string name_;
  std::string shortName_;
  unsigned int bit_ = 0u;
  std::vector<DetectorGroup> groups_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_HitPattern_H */
