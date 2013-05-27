/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Tamotsu Sato, Hirokazu Odaka                       *
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

#ifndef COMPTONSOFT_ActivationPickUpData_H
#define COMPTONSOFT_ActivationPickUpData_H 1

#include "StandardPickUpData.hh"

#include <vector>
#include <map>
#include <boost/cstdint.hpp>

#include "IsotopeInfo.hh"

class G4VTouchable;
class G4VPhysicalVolume;
class G4Ions;
class G4VAnalysisManager;

namespace comptonsoft {


/**
 * PickUpData module for radioactivation
 *
 * @author Tamotsu Sato, Hirokazu Odaka
 * @date 2011-07-28 | T. Sato  | for ANLNext
 * @date 2012-02-06 | H. Odaka | code cleanup
 * @date 2012-06-13 | T. Sato & H. Odaka
 */
class ActivationPickUpData : public anlgeant4::StandardPickUpData
{
  DEFINE_ANL_MODULE(ActivationPickUpData, 1.2);

  typedef std::map<std::string, int> volume_map_t;
  typedef std::map<int64_t, IsotopeInfo> data_map_t;
public:
  ActivationPickUpData();
  virtual ~ActivationPickUpData();
  
  virtual anl::ANLStatus mod_startup();
  
  virtual void RunAct_begin(const G4Run*);
  virtual void RunAct_end(const G4Run* aRun);
  virtual void TrackAct_begin(const G4Track* aTrack);
  virtual void StepAct(const G4Step* aStep, G4Track* aTrack);

  virtual void CreateUserActions();
  
protected:
  void SetInitialEnergy(double var) { m_InitialEnergy = var; }
  void Fill(const G4Ions* nucleus,
            const G4VTouchable* touchable,
            double posx, double posy, double posz);
  void OutputVolumeInfo(const std::string& fileName="");
  void OutputSummary(const std::string& fileName, int numberOfRun);
  
  int NumberOfVolumes();
  std::string VolumeName(int index);
  
private:
  G4VAnalysisManager* m_AnalysisManager;
  std::string m_FileNameBase;
  
  double m_InitialEnergy;
      
  volume_map_t m_VolumeMap;
  std::vector<std::string> m_VolumeArray;
  std::vector<data_map_t> m_RIMapVector;
};
  
}

#endif /* COMPTONSOFT_ActivationPickUpData_H */
