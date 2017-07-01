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

#ifndef COMPTONSOFT_ActivationUserActionAssembly_H
#define COMPTONSOFT_ActivationUserActionAssembly_H 1

#include "StandardUserActionAssembly.hh"

#include <cstdint>
#include <memory>
#include <vector>
#include <map>

#include "IsotopeInfo.hh"

class G4VTouchable;
class G4VPhysicalVolume;
class G4Ions;
class G4VAnalysisManager;

namespace comptonsoft {

/**
 * UserActionAssembly module for radioactivation
 *
 * @author Tamotsu Sato, Hirokazu Odaka
 * @date 2011-07-28 | T. Sato  | for ANLNext
 * @date 2012-02-06 | H. Odaka | code cleanup
 * @date 2012-06-13 | T. Sato & H. Odaka
 * @date 2015-06-01 | Hiro Odaka
 * @date 2016-06-29 | Hiro Odaka | modify detection methods
 * @date 2017-04-25 | Hiro Odaka | support both detection methods
 * @date 2017-07-29 | Hiro Odaka | new design of VUserActionAssembly
 * @date 2017-07-29 | Hiro Odaka | use floating level of isotope.
 */
class ActivationUserActionAssembly : public anlgeant4::StandardUserActionAssembly
{
  DEFINE_ANL_MODULE(ActivationUserActionAssembly, 4.0);

  typedef std::map<std::string, int> volume_map_t;
  typedef std::map<int64_t, IsotopeInfo> data_map_t;
public:
  ActivationUserActionAssembly();
  virtual ~ActivationUserActionAssembly();
  
  anl::ANLStatus mod_startup() override;
  
  void RunActionAtBeginning(const G4Run*) override;
  void RunActionAtEnd(const G4Run* run) override;
  void TrackActionAtBeginning(const G4Track* track) override;
  void SteppingAction(const G4Step* step) override;

  void createUserActions() override;
  
protected:
  void SetInitialEnergy(double var) { m_InitialEnergy = var; }
  void Fill(const G4Ions* nucleus,
            const G4VTouchable* touchable,
            double posx, double posy, double posz);
  void OutputVolumeInfo(const std::string& filename="");
  void OutputSummary(const std::string& filename, int numberOfRun);
  
  int NumberOfVolumes();
  std::string VolumeName(int index);
  
private:
  std::unique_ptr<G4VAnalysisManager> m_AnalysisManager;
  std::string m_FilenameBase;
  bool m_DetectionByGeneration;
  std::vector<std::string> m_ProcessesToDetect;
  double m_LifetimeLimit;
  
  double m_InitialEnergy;
      
  volume_map_t m_VolumeMap;
  std::vector<std::string> m_VolumeArray;
  std::vector<data_map_t> m_RIMapVector;
};
  
} /* namespace comptonsoft */

#endif /* COMPTONSOFT_ActivationUserActionAssembly_H */
