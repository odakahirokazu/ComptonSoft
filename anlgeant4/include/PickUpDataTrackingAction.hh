/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Masanobu Ozaki, Shin Watanabe, Hirokazu Odaka      *
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

#ifndef ANLGEANT4_PickUpDataTrackingAction_H
#define ANLGEANT4_PickUpDataTrackingAction_H 1

#include "G4UserTrackingAction.hh"

namespace anlgeant4
{

class VPickUpData;

/**
 * User TrackingAction class for PickUpData
 * @author M. Ozaki, T. Takahashi, S. Watanabe, H. Odaka
 * @date 2001-02-xx (M. Ozaki)
 * @date 2002-04-29 (S. Watanabe)
 * @date 2012-05-30 (H. Odaka)
 */
class PickUpDataTrackingAction : public G4UserTrackingAction
{
public:
  explicit PickUpDataTrackingAction(VPickUpData* pud);
  ~PickUpDataTrackingAction();

  void PreUserTrackingAction(const G4Track* aTrack);
  void PostUserTrackingAction(const G4Track* aTrack);

private:
  VPickUpData* pickup_data;
};

}

#endif /* ANLGEANT4_PickUpDataTrackingAction_H */
