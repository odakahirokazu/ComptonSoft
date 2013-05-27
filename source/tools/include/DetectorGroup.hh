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
 */

class DetectorGroup
{
public:
  const std::string& Name() const { return name; }

  void setName(const std::string& str) { name = str; }
  void add(int id) { detector_id.insert(id); }

  /**
   * return true if this group includes the given detector ID.
   * @param id detector ID
   */ 
  bool isMember(int id) const
  { return (detector_id.count(id)==1) ? true : false; }
  
  void print() const;

private:
  std::string name;
  std::set<int> detector_id;
};

}

#endif /* COMPTONSOFT_DetectorGroup_H */
