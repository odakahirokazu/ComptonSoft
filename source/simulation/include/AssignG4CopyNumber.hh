/*************************************************************************
 *                                                                       *
 * Copyright (c) 2012 Hirokazu Odaka                                     *
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

#ifndef COMPTONSOFT_AssignG4CopyNumber_H
#define COMPTONSOFT_AssignG4CopyNumber_H 1

#include "BasicModule.hh"

namespace comptonsoft {


/**
 * Assign copy number according to a name of physical volume.
 *
 * @author Hirokazu Odaka
 * @date 2012-07-04
 */
class AssignG4CopyNumber : public anl::BasicModule
{
  DEFINE_ANL_MODULE(AssignG4CopyNumber, 0.0);
public:
  AssignG4CopyNumber();
  ~AssignG4CopyNumber();

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_bgnrun();

private:
  std::string m_FileName;
};

}

#endif /* COMPTONSOFT_AssignG4CopyNumber_H */
