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

#ifndef COMPTONSOFT_HXISGDSensitiveDetector_H
#define COMPTONSOFT_HXISGDSensitiveDetector_H 1

#include <map>
#include <set>

#include "VHXISGDSensitiveDetector.hh"

/**
 * A sensitive detector class of Compton Soft.
 * This implementaion is based on comptonsoft::SimpleDetPickUpData.
 * @author Hirokazu Odaka
 * @date 2012-06-27
 */
class HXISGDSensitiveDetector : public VHXISGDSensitiveDetector
{
public:
  /**
   * a constructor
   * @param name name of a logical volume associated with this sensitive detector
   */
  explicit HXISGDSensitiveDetector(G4String name);
  
  /**
   * asociate a detector ID with a string identifier of volume hierarchy.
   * @param detid detector ID
   * @param key string identifier of volume hierarchy. e.g. /World/DetectorUnit/SiModule/CdTeStrip1/CdTeWafer
   */
  void RegisterDetectorID(int detid, G4String key);

  /**
   * @return a detector ID associated with the given key.
   */
  G4int GetDetectorID(G4String key);

  /**
   * @return a detector ID associated with the given touchable.
   */
  G4int GetDetectorID(const G4VTouchable* touchable);

private:
  std::map<G4String, int> detector_map;
};


inline void HXISGDSensitiveDetector::RegisterDetectorID(int detid, G4String key)
{
  detector_map[key] = detid;
  InsertIntoPositionCalculationSet(detid);
}


inline G4int HXISGDSensitiveDetector::GetDetectorID(G4String key)
{
  std::map<G4String, int>::iterator it = detector_map.find(key);
  if (it == detector_map.end()) {
    return -1;
  }
  return (*it).second;
}


inline G4int HXISGDSensitiveDetector::GetDetectorID(const G4VTouchable* touchable)
{
  return GetDetectorID(MakeKey(touchable));
}

#endif /* COMPTONSOFT_HXISGDSensitiveDetector_H */
