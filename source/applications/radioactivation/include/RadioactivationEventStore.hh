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

#ifndef COMPTONSOFT_RadioactivationEventStore_H
#define COMPTONSOFT_RadioactivationEventStore_H 1

#include "CSEventStore.hh"

#include <G4ThreeVector.hh>
#include "IsotopeInfo.hh"

namespace comptonsoft
{

using RadioactivationInfo = std::tuple<IsotopeInfo, int, G4ThreeVector>;

/**
 * Raw hit store module for ComptonSoft
 * @author Hirokazu Odaka
 * @date 2026-06-16 | Hirokazu Odaka
 */
class RadioactivationEventStore : public CSEventStore
{
  DEFINE_ANL_MODULE(RadioactivationEventStore, 1.0);
public:
  RadioactivationEventStore();
  virtual ~RadioactivationEventStore();

  void initialize_run(int run_id, int num_events) override;

  void insert_radioactivation(size_t event_index, const RadioactivationInfo& info);
  const std::vector<RadioactivationInfo>& get_radioactivations() const;

private:
  std::vector<std::vector<RadioactivationInfo>> radioactivations_vector_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_RadioactivationEventStore_H */
