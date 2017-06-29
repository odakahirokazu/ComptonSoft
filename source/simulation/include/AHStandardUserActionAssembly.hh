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

#include "StandardUserActionAssembly.hh"

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
class AHStandardUserActionAssembly : public anlgeant4::StandardUserActionAssembly
{
  DEFINE_ANL_MODULE(AHStandardUserActionAssembly, 2.0);
public:
  AHStandardUserActionAssembly();
  virtual ~AHStandardUserActionAssembly();
  
  anl::ANLStatus mod_startup() override;
  anl::ANLStatus mod_init() override;
  anl::ANLStatus mod_ana() override;
  anl::ANLStatus mod_exit() override;
  
  void RunActionAtBeginning(const G4Run*) override;
  void RunActionAtEnd(const G4Run*) override;

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
  
} /* comptonsoft */

#endif /* COMPTONSOFT_AHStandardPickUpData_H */
