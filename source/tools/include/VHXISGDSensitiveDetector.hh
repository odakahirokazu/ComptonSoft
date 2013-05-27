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

#ifndef COMPTONSOFT_VHXISGDSensitiveDetector_H
#define COMPTONSOFT_VHXISGDSensitiveDetector_H 1

#include <set>

#include "G4VSensitiveDetector.hh"

namespace comptonsoft
{
class DetectorManager;
}

/**
 * A sensitive detector class of Compton Soft.
 * This implementaion is based on comptonsoft::SimpleDetPickUpData.
 * @author Hirokazu Odaka
 * @date 2011-04-04
 */
class VHXISGDSensitiveDetector : public G4VSensitiveDetector
{
public:
  /**
   * a constructor
   * @param name name of a logical volume associated with this sensitive detector
   */
  explicit VHXISGDSensitiveDetector(G4String name);
  
  /**
   * a mandatory method to process geant4 raw hits
   */
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);

  /**
   * @return a detector ID associated with the given key.
   */
  G4int GetDetectorID(G4String key);

  /**
   * @return a detector ID associated with the given touchable.
   */
  virtual int GetDetectorID(const G4VTouchable* touchable) = 0;

  void SetDetectorManager(comptonsoft::DetectorManager* man)
  { detector_manager = man; }

  /**
   * If you call this method with true, hit position is determined according to the Geant4 geometry.
   * This class assigns position inforamtion from the Geant4 geometry to detector objects.
   * By default, the position information in the detector objects should be set.
   */
  void SetPositionCalculation(bool v=true) { position_calculation = v; }

  /**
   * If you call this method with true, this class prints a warning message when an unregistered detector detects any hits.
   * This setting is useful for detecting an incomplete registration of the sensitive detectors.
   * With the parameter of false, this function is disabled, and such unregistered detectors are ignored.
   * If you have any passive volumes associated with this sensitive logical volume, you should disable this function.
   */
  void SetSDCheck(bool v=true) { sd_check = v; }

protected:
  G4String MakeKey(const G4VTouchable* touchable);
  void InsertIntoPositionCalculationSet(int detid)
  { position_calculation_set.insert(detid); }
  
private:
  comptonsoft::DetectorManager* detector_manager;
  bool position_calculation;
  std::set<int> position_calculation_set;
  bool sd_check;
};

#endif /* COMPTONSOFT_VHXISGDSensitiveDetector_H */
