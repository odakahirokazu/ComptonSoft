#include "MyMainGeometry.hh"
#include "MyDetectorConstruction.hh"

namespace mygeometry
{

G4VUserDetectorConstruction* MyMainGeometry::create()
{
  G4VUserDetectorConstruction* theGeometry = new MyDetectorConstruction;
  return theGeometry;
}

} /* namespace sgdgeometry */
