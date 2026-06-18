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

#ifndef COMPTONSOFT_CSRawHitStore_H
#define COMPTONSOFT_CSRawHitStore_H 1

#include "VEventStore.hh"

#include "DetectorHit.hh"

namespace comptonsoft
{

/**
 * Raw hit store module for ComptonSoft
 * @author Hirokazu Odaka
 * @date 2026-06-16 | Hirokazu Odaka
 */
class CSRawHitStore : public anlgeant4::VEventStore
{
  DEFINE_ANL_MODULE(CSRawHitStore, 1.0);
public:
  CSRawHitStore();
  virtual ~CSRawHitStore();

  void initializeRun(int runID, int num_events) override;
  void initializeEvent(int eventID) override;

  void insertHit(const DetectorHit& hit);
  void insertHit(DetectorHit&& hit);

  const std::vector<DetectorHit>& getHits() const;

private:
  std::vector<std::vector<DetectorHit>> hits_vector_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CSRawHitStore_H */
