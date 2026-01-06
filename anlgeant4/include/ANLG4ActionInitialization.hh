#ifndef ANLGEANT4_ANLG4ActionInitialization_H
#define ANLGEANT4_ANLG4ActionInitialization_H 1

#include "G4VUserActionInitialization.hh"

namespace anlgeant4
{

class VANLPrimaryGen;
class VUserActionAssembly;

class ANLG4ActionInitialization : public G4VUserActionInitialization
{
public:
  ANLG4ActionInitialization(VANLPrimaryGen* primaryGen, VUserActionAssembly* userActionAssembly);
  virtual ~ANLG4ActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

  void RegisterUserAction(G4UserRunAction* action) const { SetUserAction(action); }
  void RegisterUserAction(G4UserEventAction* action) const { SetUserAction(action); }
  void RegisterUserAction(G4UserTrackingAction* action) const { SetUserAction(action); }
  void RegisterUserAction(G4UserSteppingAction* action) const { SetUserAction(action); }
  void RegisterUserAction(G4UserStackingAction* action) const { SetUserAction(action); }

private:
  VANLPrimaryGen* m_PrimaryGen;
  VUserActionAssembly* m_UserActionAssembly;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_ANLG4ActionInitialization_H */