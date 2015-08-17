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

#ifndef COMPTONSOFT_DetectorGroup_H
#define COMPTONSOFT_DetectorGroup_H 1

#include <string>
#include <set>

namespace comptonsoft {

/**
 * A class of a detector group.
 * @author Hirokazu Odaka
 * @date 2014-11-18
 */
class DetectorGroup
{
public:
  DetectorGroup();
  explicit DetectorGroup(const std::string& name);
  ~DetectorGroup();
  DetectorGroup(const DetectorGroup&) = default;
  DetectorGroup(DetectorGroup&&) = default;
  DetectorGroup& operator=(const DetectorGroup&) = default;
  DetectorGroup& operator=(DetectorGroup&&) = default;

  std::string Name() const { return name_; }
  void setName(const std::string& v) { name_ = v; }

  void add(int detectorID) { detectorIDs_.insert(detectorID); }

  /**
   * return true if this group includes the given detector ID.
   * @param detctorID detector ID
   */ 
  bool isMember(int detectorID) const
  { return (detectorIDs_.count(detectorID)==1); }
  
  void print() const;

private:
  std::string name_;
  std::set<int> detectorIDs_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DetectorGroup_H */
