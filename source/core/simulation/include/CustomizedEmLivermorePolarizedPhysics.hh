//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/**
 * 2018-06-22 | Hirokazu Odaka
 * This physics constructor is based on G4EmLivermorePolarizedPhysics.
 * 
 */

#ifndef CustomizedEmLivermorePolarizedPhysics_H
#define CustomizedEmLivermorePolarizedPhysics_H 1

#include "G4VPhysicsConstructor.hh"
#include "G4EmParticleList.hh"
#include "globals.hh"

class CustomizedEmLivermorePolarizedPhysics : public G4VPhysicsConstructor
{
public:

  explicit CustomizedEmLivermorePolarizedPhysics(G4int ver=1, const G4String& name="");
  ~CustomizedEmLivermorePolarizedPhysics();

  void ConstructParticle() override;
  void ConstructProcess() override;

  void setEMOptions(bool fluo, bool auger, bool pixe);
  void setElectronRangeParameters(double ratio, double finalRange)
  {
    e_range_ratio_ = ratio;
    e_final_range_ = finalRange;
  }

private:
  G4int verbose;
  G4EmParticleList partList;

  double e_range_ratio_;
  double e_final_range_;
};

#endif /* CustomizedEmLivermorePolarizedPhysics_H */
