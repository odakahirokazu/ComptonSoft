#include "ANLG4ActionInitialization.hh"
#include "VANLPrimaryGen.hh"
#include "VUserActionAssembly.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

namespace anlgeant4
{

ANLG4ActionInitialization::ANLG4ActionInitialization(VANLPrimaryGen* primaryGen, VUserActionAssembly* userActionAssembly)
  : G4VUserActionInitialization(),
    m_PrimaryGen(primaryGen),
    m_UserActionAssembly(userActionAssembly)
{
}

ANLG4ActionInitialization::~ANLG4ActionInitialization()
{
}

void ANLG4ActionInitialization::BuildForMaster() const
{
  if (m_UserActionAssembly) {
    // Thread-specific initialization (e.g. for G4AnalysisManager Booking)
    m_UserActionAssembly->buildUserActions();

    ANLG4ActionInitialization* nonConstThis = const_cast<ANLG4ActionInitialization*>(this);
    m_UserActionAssembly->registerUserActions(nonConstThis, true);
  }
}

void ANLG4ActionInitialization::Build() const
{
  if (m_PrimaryGen) {
    SetUserAction(m_PrimaryGen->create());
  }
  
  if (m_UserActionAssembly) {
    // Thread-specific initialization (e.g. for G4AnalysisManager Booking)
    m_UserActionAssembly->buildUserActions();

    ANLG4ActionInitialization* nonConstThis = const_cast<ANLG4ActionInitialization*>(this);
    m_UserActionAssembly->registerUserActions(nonConstThis, false);
  }
}

} /* namespace anlgeant4 */
