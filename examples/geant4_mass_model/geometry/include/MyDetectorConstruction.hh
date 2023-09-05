#ifndef MYGEOMETRY_MyDetectorConstruction_H
#define MYGEOMETRY_MyDetectorConstruction_H 1

#include "G4VPhysicalVolume.hh"
#include "G4VUserDetectorConstruction.hh"

namespace mygeometry {

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  MyDetectorConstruction() = default;
  G4VPhysicalVolume* Construct() override;
};

} /* namespace mygeometry */

#endif /* MYGEOMETRY_MyDetectorConstruction_H */
