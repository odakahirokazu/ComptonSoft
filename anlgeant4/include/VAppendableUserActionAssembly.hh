/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Shin Watanabe, Hirokazu Odaka                      *
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

#ifndef ANLGEANT4_VAppendableUserActionAssembly_H
#define ANLGEANT4_VAppendableUserActionAssembly_H 1

#include "VUserActionAssembly.hh"

namespace anlgeant4
{

/**
 * Virtual appendable UserActionAssembly module
 * @author Hirokazu Odaka
 * @date 2017-06-29
 */
class VAppendableUserActionAssembly : public VUserActionAssembly
{
  DEFINE_ANL_MODULE(VAppendableUserActionAssembly, 5.0);
public:
  VAppendableUserActionAssembly();
  virtual ~VAppendableUserActionAssembly();

  anlnext::ANLStatus mod_pre_initialize() override;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VAppendableUserActionAssembly_H */
