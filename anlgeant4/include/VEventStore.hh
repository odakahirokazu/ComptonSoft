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

#ifndef ANLGEANT4_VEventStore_H
#define ANLGEANT4_VEventStore_H 1

#include <anlnext/BasicModule.hh>

#include "InitialInformation.hh"


namespace anlgeant4
{

/**
 * Virtual raw hit store module
 * @author Hirokazu Odaka
 * @date 2026-05-26 | Hirokazu Odaka | for parallel simulation design
 */
class VEventStore : public anlnext::BasicModule, public InitialInformation
{
  DEFINE_ANL_MODULE(VEventStore, 1.0);
public:
  VEventStore();
  virtual ~VEventStore();

  virtual void initializeRun(int runID, int num_events);
  virtual void finalizeRun();
  virtual void initializeEvent(int eventID);
  virtual void finalizeEvent() {}

  anlnext::ANLStatus mod_analyze() override;

protected:
  size_t read_index() const { return read_index_; }

private:
  std::vector<int> eventid_vector_;
  size_t read_index_ = 0;
  bool postprocess_ready_ = false;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_VEventStore_H */
