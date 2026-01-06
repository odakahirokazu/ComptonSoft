#ifndef ANLGEANT4_Geant4MTBody_H
#define ANLGEANT4_Geant4MTBody_H 1

#include <string>
#include <memory>
#include <anlnext/BasicModule.hh>
#include "globals.hh"

namespace CLHEP
{
class HepRandomEngine;
}

namespace anlgeant4
{

class ANLG4MTRunManager;
class VANLPrimaryGen;
class VUserActionAssembly;

class Geant4MTBody : public anlnext::BasicModule
{
  DEFINE_ANL_MODULE(Geant4MTBody, 1.0);
public: 
  Geant4MTBody();
  ~Geant4MTBody();

  anlnext::ANLStatus mod_define() override;
  anlnext::ANLStatus mod_initialize() override;
  anlnext::ANLStatus mod_begin_run() override;
  anlnext::ANLStatus mod_analyze() override;
  anlnext::ANLStatus mod_end_run() override;
  anlnext::ANLStatus mod_finalize() override;

  void set_verbose_level(G4int v) { m_VerboseLevel = v; }
  G4int get_verbose_level() { return m_VerboseLevel; }
  
protected:
  virtual void initialize_random_generator();
  virtual void set_user_initializations();
  virtual void set_user_primary_generator_action();
  virtual void set_user_defined_actions();
  virtual void apply_commands();

private:
  std::unique_ptr<ANLG4MTRunManager> m_G4RunManager;
  std::unique_ptr<CLHEP::HepRandomEngine> m_RandomEnginePtr;
  int m_EventIndex = 0;
  
  std::string m_RandomEngine;
  int m_RandomInitMode;
  int m_RandomSeed1;
  bool m_OutputRandomStatus;
  std::string m_RandomInitialStatusFileName;
  std::string m_RandomFinalStatusFileName;

  int m_VerboseLevel;
  std::vector<std::string> m_UserCommands;

  int m_NumberOfThreads;
  int m_NEvents;
  
  VANLPrimaryGen* m_PrimaryGen;
  VUserActionAssembly* m_UserActionAssembly;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_Geant4MTBody_H */
