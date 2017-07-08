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

#ifndef COMPTONSOFT_OutputSimXPrimaries_H
#define COMPTONSOFT_OutputSimXPrimaries_H 1

#include "BasicModule.hh"

namespace comptonsoft {


class OutputSimXPrimaries : public anl::BasicModule
{
  DEFINE_ANL_MODULE(OutputSimXPrimaries, 0.1);
public:
  OutputSimXPrimaries();
  ~OutputSimXPrimaries() {}
  
  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  
private:
  std::string m_FileName;
  double m_Area;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_OutputSimXPrimaries_H */
