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

#include "DetectorGroup.hh"
#include <iostream>
#include <iomanip>

namespace comptonsoft {

void DetectorGroup::print() const
{
  std::cout << "Detector group: " << name << std::endl;
  std::set<int>::iterator it;
  int i = 0;
  for (it=detector_id.begin(); it!=detector_id.end(); it++) {
    if (i%10==0) { std::cout << "    "; }
    std::cout << std::setw(4) << *it;
    std::cout << ' ';
    if (i%10==9) { std::cout << '\n'; }
    i++;
  }
  std::cout << std::endl;
}

}
