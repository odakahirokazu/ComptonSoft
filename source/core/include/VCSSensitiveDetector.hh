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

#ifndef COMPTONSOFT_VCSSensitiveDetector_H
#define COMPTONSOFT_VCSSensitiveDetector_H 1

#include "G4VSensitiveDetector.hh"
#include <set>

namespace comptonsoft
{

class DetectorSystem;

/**
 * A sensitive detector class of Compton Soft.
 * This implementaion is based on comptonsoft::SimpleDetPickUpData.
 * @author Hirokazu Odaka
 * @date 2011-04-04
 * @date 2014-11-14
 */
class VCSSensitiveDetector : public G4VSensitiveDetector
{
public:
  /**
   * a constructor
   * @param name name of a logical volume associated with this sensitive detector
   */
  explicit VCSSensitiveDetector(G4String name);
  ~VCSSensitiveDetector();
  
  /**
   * a mandatory method to process geant4 raw hits
   */
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);

  void SetDetectorSystem(comptonsoft::DetectorSystem* detectorSystem)
  { detectorSystem_ = detectorSystem; }

  /**
   * If you call this method with true, hit position is determined according to the Geant4 geometry.
   * This class assigns position inforamtion from the Geant4 geometry to detector objects.
   * By default, the position information in the detector objects should be set.
   */
  void SetPositionCalculation(bool v=true)
  { positionCalculation_ = v; }

  /**
   * If you call this method with true, this class prints a warning message when an unregistered detector detects any hits.
   * This setting is useful for detecting an incomplete registration of the sensitive detectors.
   * With the parameter of false, this function is disabled, and such unregistered detectors are ignored.
   * If you have any passive volumes associated with this sensitive logical volume, you should disable this function.
   */
  void SetSDCheck(bool v=true)
  { SDCheck_ = v; }

  void SetLayerOffset(int v) { layerOffset_ = v; }
  int LayerOffset() const { return layerOffset_; }
  
  /**
   * @return a detector ID associated with the given touchable.
   */
  virtual int GetDetectorID(const G4VTouchable* touchable) = 0;

protected:
  void InsertIntoPositionCalculationSet(int detectorID)
  { positionCalculationSet_.insert(detectorID); }
  G4String HierarchyString(const G4VTouchable* touchable) const;
  
private:
  bool positionCalculation_;
  bool SDCheck_;
  int layerOffset_;
  comptonsoft::DetectorSystem* detectorSystem_;
  std::set<int> positionCalculationSet_;
};

inline G4String VCSSensitiveDetector::
HierarchyString(const G4VTouchable* touchable) const
{
  G4String key;
  for(G4int i=touchable->GetHistoryDepth()-layerOffset_; i>=0; --i) {
    G4String volumeName = touchable->GetVolume(i)->GetName();
    key += volumeName + (i==0 ? "" : "/");
  }
  return key;
}

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_VCSSensitiveDetector_H */
