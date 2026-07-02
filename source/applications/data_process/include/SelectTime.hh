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

#ifndef COMPTONSOFT_SelectTime_H
#define COMPTONSOFT_SelectTime_H 1

#include <anlnext/BasicModule.hh>

namespace comptonsoft {

class ReadDataFile;

/**
 * @author Hirokazu Odaka
 */
class SelectTime : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(SelectTime, 1.1);
public:
  SelectTime();
  ~SelectTime() = default;

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  const ReadDataFile* m_ReadDataModule;
  int m_Time0;
  int m_Time1;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_SelectTime_H */
