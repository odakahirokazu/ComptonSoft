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

#ifndef COMPTONSOFT_WriteHXIEventFITS_H
#define COMPTONSOFT_WriteHXIEventFITS_H 1

#include "VCSModule.hh"
#include <memory>
#include "HXIEventFITS.hh"

namespace anlgeant4 {
class InitialInformation;
}

namespace comptonsoft {

class CSHitCollection;

/**
 * ANL module to read HXI event ROOT files.
 *
 * @author Hirokazu Odaka
 * @date 2015-12-02
 */
class WriteHXIEventFITS : public VCSModule
{
  DEFINE_ANL_MODULE(WriteHXIEventFITS, 0.0);
public:
  WriteHXIEventFITS();
  ~WriteHXIEventFITS();
  
  anl::ANLStatus mod_define() override;
  anl::ANLStatus mod_initialize() override;
  anl::ANLStatus mod_analyze() override;
  anl::ANLStatus mod_finalize() override;
  
private:
  std::string m_Filename;

  const CSHitCollection* m_HitCollection;
  const anlgeant4::InitialInformation* m_InitialInfo;
  std::unique_ptr<astroh::hxi::EventFITSWriter> m_EventWriter;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_WriteHXIEventFITS_H */
