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

#ifndef COMPTONSOFT_ConstructChannelTable_H
#define COMPTONSOFT_ConstructChannelTable_H 1

#include <vector>
#include <string>

#include "VCSModule.hh"

#include "StripPair.hh"
#include "ChannelTable.hh"

namespace comptonsoft {

class ConstructChannelTable : public VCSModule
{
  DEFINE_ANL_MODULE(ConstructChannelTable, 1.0);
public:
  ConstructChannelTable();
  ~ConstructChannelTable() {}

  anl::ANLStatus mod_startup();
  anl::ANLStatus mod_init();
  anl::ANLStatus mod_exit();

private:
  std::vector<comptonsoft::ChannelTable*> m_ChannelTableVec;
  std::string m_FileName;
};

}

#endif /* COMPTONSOFT_ConstructChannelTable_H */
