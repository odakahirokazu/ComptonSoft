/**
 * @file DummyDetectorConstruction.cc
 * @brief source file of class DummyDetectorConstruction
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#include "DummyDetectorConstruction.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

namespace comptonsoft {

G4VPhysicalVolume* DummyDetectorConstruction::Construct()
{
  G4Material* vacuum = 
    G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  G4Box* solidWorld= new G4Box("World",
                               1.0*CLHEP::m,
                               1.0*CLHEP::m,
                               1.0*CLHEP::m);
  G4LogicalVolume* logicWorld = 
    new G4LogicalVolume(solidWorld, vacuum, "World_Logical");
  
  G4VPhysicalVolume* pWorld = 
    new G4PVPlacement(0,
                      G4ThreeVector(0.0, 0.0, 0.0),
                      logicWorld,
                      "World",
                      0, // mother
                      false,
                      0);
  return pWorld;
}

} // namespace comptonsoft
