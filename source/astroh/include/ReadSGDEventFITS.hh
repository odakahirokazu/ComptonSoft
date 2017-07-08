/*************************************************************************
 *                                                                       *
 * Copyright (c) 2013 Hirokazu Odaka                                     *
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

#ifndef COMPTONSOFT_ReadSGDEventFITS_H
#define COMPTONSOFT_ReadSGDEventFITS_H 1

#include "VCSModule.hh"
#include "InitialInformation.hh"
#include <memory>
#include "SGDEventFITS.hh"

namespace comptonsoft {

/**
 * ANL module to read SGD event ROOT files.
 *
 * @author Hirokazu Odaka
 * @date 2014-09-05
 * @date 2016-12-20
 * @date 2017-02-06 | access to event time
 */
class ReadSGDEventFITS : public VCSModule, public anlgeant4::InitialInformation
{
  DEFINE_ANL_MODULE(ReadSGDEventFITS, 1.1);
public:
  ReadSGDEventFITS();
  ~ReadSGDEventFITS();
  
  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;

  double EventTime() const { return m_EventTime; }
  
private:
  std::string m_Filename;
  int m_CCID;
  bool m_PseudoPass;
  bool m_VetoEnabled;
  bool m_StandardSelectionEnabled;

  std::unique_ptr<astroh::sgd::EventFITSReader> m_EventReader;
  Long64_t m_NumEvents;
  Long64_t m_Index;

  double m_EventTime;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ReadSGDEventFITS_H */
