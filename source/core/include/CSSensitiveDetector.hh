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

#ifndef COMPTONSOFT_CSSensitiveDetector_H
#define COMPTONSOFT_CSSensitiveDetector_H 1

#include "VCSSensitiveDetector.hh"
#include <map>

namespace comptonsoft
{

class DetectorSystem;

/**
 * A sensitive detector class of Compton Soft.
 * This implementaion is based on comptonsoft::SimpleDetPickUpData.
 * @author Hirokazu Odaka
 * @date 2011-04-04
 */
template <typename DetectorKeyType>
class CSSensitiveDetector : public VCSSensitiveDetector
{
public:
  CSSensitiveDetector(const std::string& name)
    : VCSSensitiveDetector(name)
  {
  }
  
  /**
   * asociate a detector ID with a string identifier of volume hierarchy.
   * @param detid detector ID
   * @param key string identifier of volume hierarchy. e.g. /World/DetectorUnit/SiModule/CdTeStrip1/CdTeWafer
   */
  void RegisterDetectorID(int detectorID, DetectorKeyType key)
  {
    detectorMap_[key] = detectorID;
    InsertIntoPositionCalculationSet(detectorID);
  }

  int GetDetectorID(const G4VTouchable* touchable) override
  {
    DetectorKeyType key = MakeKey(touchable);
    return GetDetectorIDByKey(key);
  }
  
  /**
   * @return a detector ID associated with the given key.
   */
  int GetDetectorIDByKey(DetectorKeyType key)
  {
    auto it = detectorMap_.find(key);
    if (it == detectorMap_.end()) {
      return -1;
    }
    return (*it).second;
  }

protected:
  DetectorKeyType MakeKey(const G4VTouchable* touchable) const
  {
    return HierarchyString(touchable);
  }
  
private:
  std::map<DetectorKeyType, int> detectorMap_;
};

template<>
G4String CSSensitiveDetector<G4String>::MakeKey(const G4VTouchable* touchable) const
{
  return HierarchyString(touchable);
}

template<>
int CSSensitiveDetector<int>::MakeKey(const G4VTouchable* touchable) const
{
  int copyNo = touchable->GetVolume(LayerOffset())->GetCopyNo();
  return copyNo;
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_CSSensitiveDetector_H */
