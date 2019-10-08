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

#ifndef COMPTONSOFT_AssignTime_H
#define COMPTONSOFT_AssignTime_H 1

#include "VCSModule.hh"
#include <list>
#include "ReadEventTree.hh"

namespace comptonsoft {

class CSHitCollection;


/**
 * @author Hirokazu Odaka
 * @date 2019-01-30
 */
class AssignTime : public VCSModule
{
  DEFINE_ANL_MODULE(AssignTime, 1.0);
public:
  AssignTime();
  ~AssignTime();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  void sampleEventTimes();

private:
  int64_t m_NumEvents = -1;
  double m_Time0 = 0.0;
  double m_Time1 = 0.0;
  int m_Seed = -1;
  bool m_SortTime = false;
  bool m_RandomizeTime = true;
  double m_CountRate = 0.0;
  double m_Exposure = 0.0;

  CSHitCollection* m_HitCollection = nullptr;
  ReadEventTree* m_ReadEventTree = nullptr;
  std::list<double> m_TimeList;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AssignTime_H */
