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

#ifndef COMPTONSOFT_AssignTimeWithExpDistribution_H
#define COMPTONSOFT_AssignTimeWithExpDistribution_H 1

#include "VCSModule.hh"
#include <list>
#include <random>

namespace comptonsoft {

class CSHitCollection;


/**
 * @author Hirokazu Odaka
 * @date 2019-01-30
 */
class AssignTimeWithExpDistribution : public VCSModule
{
  DEFINE_ANL_MODULE(AssignTimeWithExpDistribution, 1.0);
public:
  AssignTimeWithExpDistribution();
  ~AssignTimeWithExpDistribution();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  double m_Time = 0.0;
  int m_Seed = -1;
  double m_CountRate = 0.0;
  std::mt19937 m_Engine;
  std::exponential_distribution<double> m_Distribution;

  CSHitCollection* m_HitCollection = nullptr;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AssignTime_H */
