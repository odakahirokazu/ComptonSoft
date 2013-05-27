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

#ifndef COMPTONSOFT_DetectorReadModule_H
#define COMPTONSOFT_DetectorReadModule_H 1

#include <vector>

namespace comptonsoft {

class OneASICData;

/**
 * A class of a detector readout module.
 * @author Hirokazu Odaka
 * @date 2008-xx-xx
 */

class DetectorReadModule
{
public:
  DetectorReadModule() {}
  virtual ~DetectorReadModule() {}
  
private:
  int m_ID;
  std::vector<OneASICData*> m_ASICDataVector;

public:
  void setID(int i) { m_ID = i; }
  int getID() { return m_ID; }

  void registerASICData(OneASICData* r) { m_ASICDataVector.push_back(r); }
  int NumASIC() { return m_ASICDataVector.size(); }
  OneASICData* getASICData(int i) { return m_ASICDataVector[i]; }
  std::vector<OneASICData*>::iterator ASICDataBegin() { return m_ASICDataVector.begin(); }
  std::vector<OneASICData*>::iterator ASICDataEnd() { return m_ASICDataVector.end(); }

private:
  DetectorReadModule(const DetectorReadModule&);
  DetectorReadModule& operator=(const DetectorReadModule&);
};

}

#endif /* COMPTONSOFT_DetectorReadModule_H */
