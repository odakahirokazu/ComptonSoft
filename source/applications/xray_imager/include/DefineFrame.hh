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

#ifndef COMPTONSOFT_DefineFrame_H
#define COMPTONSOFT_DefineFrame_H 1

#include "VCSModule.hh"
#include <list>
#include "AstroUnits.hh"

namespace anlgeant4 {
class InitialInformation;
}

namespace comptonsoft {

class CSHitCollection;

/**
 * @author Hirokazu Odaka
 * @date 2019-04-02
 */
class DefineFrame : public VCSModule
{
  DEFINE_ANL_MODULE(DefineFrame, 1.0);
public:
  DefineFrame();
  ~DefineFrame();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;

private:
  void sampleEventTimes();

private:
  double m_Time0 = 0.0;
  double m_FrameExposure = 1.0*anlgeant4::unit::s;

  anlgeant4::InitialInformation* m_InitialInfo = nullptr;
  CSHitCollection* m_HitCollection = nullptr;

  int m_CurrentFrameID = 0;
  int m_CurrentEventID = 0;
  int m_EventIndexInThisFrame = 0;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DefineFrame_H */
