/**
 * @file DummyDetectorConstruction.hh
 * @brief header file of class DummyDetectorConstruction
 * @author Hirokazu Odaka
 * @date 2016-05-04
 */

#ifndef COMPTONSOFT_DummyDetectorConstruction_H
#define COMPTONSOFT_DummyDetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

namespace comptonsoft {

class DummyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DummyDetectorConstruction() = default;
  virtual ~DummyDetectorConstruction() = default;
  
  G4VPhysicalVolume* Construct() override;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_DummyDetectorConstruction_H */
