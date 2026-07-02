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

#ifndef COMPTONSOFT_SetBadChannels_H
#define COMPTONSOFT_SetBadChannels_H 1

#include "VCSModule.hh"
#include <map>
#include <tuple>

namespace comptonsoft {

class SetBadChannels : public VCSModule
{
  DEFINE_ANL_MODULE(SetBadChannels, 1.0);
public:
  SetBadChannels();
  ~SetBadChannels();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;

private:
  bool set_by_file();

private:
  std::string m_FileName;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SetBadChannels_H */
