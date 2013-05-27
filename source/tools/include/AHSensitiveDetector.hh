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

#ifndef COMPTONSOFT_AHSensitiveDetector_H
#define COMPTONSOFT_AHSensitiveDetector_H 1

#include <map>

#include "VHXISGDSensitiveDetector.hh"

/**
 * A sensitive detector class of Compton Soft.
 * This implementaion is based on comptonsoft::SimpleDetPickUpData.
 * @author Hirokazu Odaka
 * @date 2012-06-27
 */
class AHSensitiveDetector : public VHXISGDSensitiveDetector
{
public:
  /**
   * a constructor
   * @param name name of a logical volume associated with this sensitive detector
   */
  explicit AHSensitiveDetector(G4String name);
  
  /**
   * asociate a detector ID with a string identifier of volume hierarchy.
   * @param detid detector ID
   * @param key string identifier of volume hierarchy. e.g. /World/DetectorUnit/SiModule/CdTeStrip1/CdTeWafer
   */
  void RegisterDetectorID(int detid, G4int copy_number);

  /**
   * @return a detector ID associated with the given key.
   */
  G4int GetDetectorID(G4int copy_number);

  /**
   * @return a detector ID associated with the given touchable.
   */
  G4int GetDetectorID(const G4VTouchable* touchable);

  void SetLayerOffset(int v) { layer_offset = v; }

protected:
  G4int MakeCopyNo(const G4VTouchable* touchable);
  
private:
  std::map<G4int, int> detector_map;
  int layer_offset;
};


inline void AHSensitiveDetector::RegisterDetectorID(int detid, G4int copy_number)
{
  detector_map[copy_number] = detid;
  InsertIntoPositionCalculationSet(detid);
}


inline int AHSensitiveDetector::GetDetectorID(G4int copy_number)
{
  std::map<G4int, int>::iterator it = detector_map.find(copy_number);
  if (it == detector_map.end()) {
    return -1;
  }
  return (*it).second;
}


inline G4int AHSensitiveDetector::GetDetectorID(const G4VTouchable* touchable)
{
  return GetDetectorID(MakeCopyNo(touchable));
}

#endif /* COMPTONSOFT_AHSensitiveDetector_H */
