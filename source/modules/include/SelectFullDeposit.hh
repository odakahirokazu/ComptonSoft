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

#ifndef COMPTONSOFT_SelectFullDeposit_H
#define COMPTONSOFT_SelectFullDeposit_H 1

#include "BasicModule.hh"

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {

class CSHitCollection;

/**
 * Select events that have complete energy deposit. (no energy escape.)
 * @author  Hirokazu Odaka
 * @date 2008-12-12
 * @date 2011-04-26
 */
class SelectFullDeposit : public anl::BasicModule
{
  DEFINE_ANL_MODULE(SelectFullDeposit, 1.1);
public:
  SelectFullDeposit() = default;
  ~SelectFullDeposit() = default;

  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_ana() override;
  
private:
  CSHitCollection* m_HitCollection;
  const anlgeant4::InitialInformation* m_InitialInfo;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SelectFullDeposit_H */


