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

#ifndef COMPTONSOFT_ReadDataFile_NT_H
#define COMPTONSOFT_ReadDataFile_NT_H 1

#include "ReadDataFile.hh"

#include <cstdint>
#include <memory>

class TChain;

namespace comptonsoft {

/**
 * Read data file: "NabeTree" format.
 * @author Hirokazu Odaka
 * @date 2011-11-20
 * @date 2014-11-25
 */
class ReadDataFile_NT : public ReadDataFile
{
  DEFINE_ANL_MODULE(ReadDataFile_NT, 3.2);
public:
  ReadDataFile_NT() = default;
  ~ReadDataFile_NT() = default;

  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;

private:
  TChain* m_Tree = nullptr;
  uint32_t m_NEvents = 0u;
  std::unique_ptr<std::unique_ptr<uint16_t[]>[]> m_ADC;
  int m_UnixTime = 0;
  // unsigned int m_LiveTime;
  // unsigned int m_IntegralLiveTime;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadDataFile_NT_H */
