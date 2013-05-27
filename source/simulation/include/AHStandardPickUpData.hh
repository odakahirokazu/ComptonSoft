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

#ifndef COMPTONSOFT_AHStandardPickUpData_H
#define COMPTONSOFT_AHStandardPickUpData_H 1

#include "StandardPickUpData.hh"

#include <vector>
#include <map>
#include <boost/cstdint.hpp>

#include "DetectorHit_sptr.hh"

class G4VPhysicalVolume;
class G4Ions;
class G4VAnalysisManager;

namespace anlgeant4 {

class InitialInformation;

}

namespace comptonsoft {

class SimXIF;
class EventReconstruction;
class CSHitCollection;


/**
 * PickUpData module for ASTRO-H
 *
 * @author Hirokazu Odaka
 * @date 2012-06-24
 */
class AHStandardPickUpData : public anlgeant4::StandardPickUpData
{
  DEFINE_ANL_MODULE(AHStandardPickUpData, 1.0);
public:
  AHStandardPickUpData();
  virtual ~AHStandardPickUpData();
  
  virtual anl::ANLStatus mod_startup();
  virtual anl::ANLStatus mod_init();
  virtual anl::ANLStatus mod_ana();
  virtual anl::ANLStatus mod_exit();
  
  virtual void RunAct_begin(const G4Run*);
  virtual void RunAct_end(const G4Run*);

protected:
  void Fill(int seqnum, int totalhit,
            comptonsoft::const_DetectorHit_sptr hit);

  void MakeEvent(const std::vector<comptonsoft::DetectorHit_sptr>& hits);

private:
  G4VAnalysisManager* m_AnalysisManager;

  CSHitCollection* m_HitCollection;
  const anlgeant4::InitialInformation* m_InitialInfo;
  SimXIF* m_SimXIF;
  EventReconstruction* m_EventReconstruction;
  

  std::string m_FileName;
  bool m_PolarizationEnable;
};
  
}

#endif /* COMPTONSOFT_AHStandardPickUpData_H */
