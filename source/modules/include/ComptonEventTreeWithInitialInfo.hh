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

#ifndef COMPTONSOFT_ComptonEventTreeWithInitialInfo_H
#define COMPTONSOFT_ComptonEventTreeWithInitialInfo_H 1

#include "ComptonEventTree.hh"

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {

/**
 * 
 * @author Hirokazu Odaka
 * @date 2014-12-01
 */
class ComptonEventTreeWithInitialInfo : public ComptonEventTree
{
  DEFINE_ANL_MODULE(ComptonEventTreeWithInitialInfo, 2.0);
public:
  ComptonEventTreeWithInitialInfo();
  ~ComptonEventTreeWithInitialInfo() = default;

  anl::ANLStatus mod_init();
  anl::ANLStatus mod_his();
  anl::ANLStatus mod_ana();

protected:
  uint64_t EventID() const;
  
private:
  const anlgeant4::InitialInformation* initialInfo_;
  bool polarizationEnable_;

  float ini_energy_ = 0.0;
  float ini_dirx_ = 0.0;
  float ini_diry_ = 0.0;
  float ini_dirz_ = 0.0;
  float ini_time_ = 0.0;
  float ini_posx_ = 0.0;
  float ini_posy_ = 0.0;
  float ini_posz_ = 0.0;
  float ini_polarx_ = 0.0;
  float ini_polary_ = 0.0;
  float ini_polarz_ = 0.0;
  float weight_ = 1.0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ComptonEventTreeWithInitialInfo_H */
