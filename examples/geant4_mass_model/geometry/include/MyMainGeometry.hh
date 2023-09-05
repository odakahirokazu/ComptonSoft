#ifndef MYGEOMETRY_MyMainGeometry_H
#define MYGEOMETRY_MyMainGeometry_H 1

#include "VANLGeometry.hh"
#include "G4VUserDetectorConstruction.hh"

namespace mygeometry {

class MyMainGeometry : public anlgeant4::VANLGeometry
{
  DEFINE_ANL_MODULE(MyMainGeometry, 1.0);
public:
  MyMainGeometry() = default;
  virtual ~MyMainGeometry() = default;

  G4VUserDetectorConstruction* create() override;
};

} /* namespace mygeometry */

#endif /* MYGEOMETRY_MyMainGeometry_HH */
