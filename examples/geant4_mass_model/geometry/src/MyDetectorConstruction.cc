#include "MyDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

namespace mygeometry
{

G4VPhysicalVolume* MyDetectorConstruction::Construct()
{
  // materials
  G4NistManager* nist_manager = G4NistManager::Instance();
  G4Material* material_galactic = nist_manager->FindOrBuildMaterial("G4_Galactic");
  G4Material* material_silicon = nist_manager->FindOrBuildMaterial("G4_Si");

  // world
  const G4double world_size = 100.0*cm;
  G4Box* world_solid = new G4Box("World_Solid", world_size*0.5, world_size*0.5, world_size*0.5);
  G4LogicalVolume* world_logical = new G4LogicalVolume(world_solid, material_galactic, "World_Logical"); 
  G4VPhysicalVolume* world_physical = new G4PVPlacement(0, G4ThreeVector(), world_logical, "World", 0, false, 0);

  // detector
  const G4ThreeVector position(0.0*mm, 0.0*mm, 0.0*mm);
  const G4double detector_size_x = 10.0*mm;
  const G4double detector_size_y = 10.0*mm;
  const G4double detector_size_z = 0.5*mm;
  
  G4Box* detector_solid = new G4Box("Detector_Solid",
                                    detector_size_x*0.5,
                                    detector_size_y*0.5,
                                    detector_size_z*0.5);
  G4LogicalVolume* detector_logical = new G4LogicalVolume(detector_solid, material_silicon, "Detector_Logical"); 
  new G4PVPlacement(0, position, detector_logical, "Detector", world_logical, false, 0);

  return world_physical;
}

} /* namespace mygeometry */
