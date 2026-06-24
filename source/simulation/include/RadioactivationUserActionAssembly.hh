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

#ifndef COMPTONSOFT_RadioactivationUserActionAssembly_H
#define COMPTONSOFT_RadioactivationUserActionAssembly_H 1

#include "StandardUserActionAssembly.hh"

#include <cstdint>
#include <memory>
#include <vector>
#include <map>
#include <mutex>

#include <G4ThreeVector.hh>

#include "IsotopeInfo.hh"

class G4TouchableHistory;
class G4VPhysicalVolume;
class G4Ions;
class G4VAnalysisManager;

namespace comptonsoft {

class RadioactivationEventStore;


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
 * @date 2022-05-20 | Hiro Odaka | Geant4-v11: The analysis manager is not owned by this class.
 * @date 2024-04-17 | Hiro Odaka | Geant4-v11.2: G4VTouchable -> G4TouchableHistory
 * @date 2026-06-24 | Hiro Odaka | ComptonSoft version 6
 */
class RadioactivationUserActionAssembly : public anlgeant4::StandardUserActionAssembly
{
  DEFINE_ANL_MODULE(RadioactivationUserActionAssembly, 6.0);
  ENABLE_PARALLEL_RUN();

private:
  typedef std::map<std::string, int> volume_map_t;
  typedef std::map<int64_t, IsotopeInfo> data_map_t;

public:
  RadioactivationUserActionAssembly();
  virtual ~RadioactivationUserActionAssembly();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_finalize() override;

  void RunActionAtEnd(const G4Run* run) override;

  bool isSteppingActionEffective() const override { return true; }
  void SteppingAction(const G4Step* step) override;

  bool isStackingActionEffective() const override { return true; }
  G4UserStackingAction* createStackingAction() const override;

protected:
  void fill(const G4Ions* nucleus, const G4TouchableHistory* touchable, const G4ThreeVector& position);
  int analyze_volume_info(const std::string& volume_name, const IsotopeInfo& isotope);
  void output_volume_info(const std::string& filename="");
  void output_summary(const std::string& filename);

private:
  std::string filename_base_;
  bool detection_by_generation_;
  std::vector<std::string> processes_to_detect_;
  double lifetime_limit_;

  RadioactivationEventStore* event_store_ = nullptr;

  std::shared_ptr<std::mutex> mutex_;
  std::shared_ptr<int> total_event_number_ptr_ = 0;
  std::shared_ptr<volume_map_t> volume_map_;
  std::shared_ptr<std::vector<std::string>> volume_vector_;
  std::shared_ptr<std::vector<data_map_t>> data_map_vector_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RadioactivationUserActionAssembly_H */
