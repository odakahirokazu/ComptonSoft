#include "Geant4MTBody.hh"

#include <ctime>
#include <boost/lexical_cast.hpp>

#include "ANLG4MTRunManager.hh"
#include "ANLG4ActionInitialization.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4UImanager.hh"

#include "VANLGeometry.hh"
#include "VANLPhysicsList.hh"
#include "VANLPrimaryGen.hh"
#include "VUserActionAssembly.hh"

using namespace anlnext;

namespace anlgeant4
{

Geant4MTBody::Geant4MTBody()
  : m_G4RunManager(new ANLG4MTRunManager),
    m_EventIndex(0),
    m_RandomEngine("MTwistEngine"),
    m_RandomInitMode(1),
    m_RandomSeed1(0),
    m_OutputRandomStatus(true),
    m_RandomInitialStatusFileName("RandomSeed_i.dat"),
    m_RandomFinalStatusFileName("RandomSeed_f.dat"),
    m_VerboseLevel(0),
    m_NumberOfThreads(2),
    m_NEvents(100),
    m_PrimaryGen(nullptr),
    m_UserActionAssembly(nullptr)
{
}

Geant4MTBody::~Geant4MTBody() = default;

ANLStatus Geant4MTBody::mod_define()
{
  register_parameter(&m_RandomEngine, "random_engine");
  register_parameter(&m_RandomInitMode, "random_initialization_mode");
  set_parameter_question("Random initialization mode (0: auto, 1: interger, 2: state file)");
  register_parameter(&m_RandomSeed1, "random_seed");
  register_parameter(&m_OutputRandomStatus, "output_random_status");
  register_parameter(&m_RandomInitialStatusFileName,
                     "random_initial_status_file");
  register_parameter(&m_RandomFinalStatusFileName,
                     "random_final_status_file");

  register_parameter(&m_VerboseLevel, "verbose");
  register_parameter(&m_UserCommands, "commands");
  
  register_parameter(&m_NumberOfThreads, "number_of_threads");
  register_parameter(&m_NEvents, "n_events");
  
  return AS_OK;
}

ANLStatus Geant4MTBody::mod_initialize()
{
  if (m_RandomInitMode==0 || m_RandomInitMode==1 || m_RandomInitMode==2) {
    initialize_random_generator();
  }
  else {
    std::cout << "Invalid value [Random initialization mode] : "
              << m_RandomInitMode << std::endl;
    return AS_QUIT_ERROR;
  }

  m_G4RunManager->SetNumberOfThreads(m_NumberOfThreads);

  set_user_initializations();
  set_user_primary_generator_action();
  set_user_defined_actions();

  m_G4RunManager->SetUserInitialization(new ANLG4ActionInitialization(m_PrimaryGen, m_UserActionAssembly));

  apply_commands();

  m_G4RunManager->Initialize();

  return AS_OK;
}

void Geant4MTBody::initialize_random_generator()
{
  if (m_RandomEngine=="MTwistEngine") {
    m_RandomEnginePtr.reset(new CLHEP::MTwistEngine);
    CLHEP::HepRandom::setTheEngine(m_RandomEnginePtr.get());
  }
  else if (m_RandomEngine=="RanecuEngine") {
    m_RandomEnginePtr.reset(new CLHEP::RanecuEngine);
    CLHEP::HepRandom::setTheEngine(m_RandomEnginePtr.get());
  }
  else if (m_RandomEngine=="HepJamesRandom") {
    m_RandomEnginePtr.reset(new CLHEP::HepJamesRandom);
    CLHEP::HepRandom::setTheEngine(m_RandomEnginePtr.get());
  }
  
  if (m_RandomInitMode==0) {
    m_RandomSeed1 = std::time(0);
    CLHEP::HepRandom::setTheSeed(m_RandomSeed1);
    std::cout << "Random seed: " << m_RandomSeed1 << std::endl;

    if (m_OutputRandomStatus) {
      CLHEP::HepRandom::saveEngineStatus(m_RandomInitialStatusFileName.c_str());
    }
  }
  else if (m_RandomInitMode==1) {
    CLHEP::HepRandom::setTheSeed(m_RandomSeed1);
    std::cout << "Random seed: " << m_RandomSeed1 << std::endl;

    if (m_OutputRandomStatus) {
      CLHEP::HepRandom::saveEngineStatus(m_RandomInitialStatusFileName.c_str());
    }
  }
  else if (m_RandomInitMode==2) {
    CLHEP::HepRandom::restoreEngineStatus(m_RandomInitialStatusFileName.c_str());
  }
}

void Geant4MTBody::set_user_initializations()
{
  VANLGeometry* geometry;
  get_module_NC("VANLGeometry", &geometry);
  G4VUserDetectorConstruction* userDetectorConstruction = geometry->create();
  m_G4RunManager->SetUserInitialization(userDetectorConstruction);
  
  VANLPhysicsList* physics;
  get_module_NC("VANLPhysicsList", &physics);
  G4VUserPhysicsList* userPhysicsList = physics->create();
  m_G4RunManager->SetUserInitialization(userPhysicsList);
}

void Geant4MTBody::set_user_primary_generator_action()
{
  get_module_NC("VANLPrimaryGen", &m_PrimaryGen);
}

void Geant4MTBody::set_user_defined_actions()
{
  get_module_NC("VUserActionAssembly", &m_UserActionAssembly);
}

void Geant4MTBody::apply_commands()
{
  using boost::format;
  using boost::str;

  G4UImanager* ui = G4UImanager::GetUIpointer();
  
  std::vector<std::string> presetCommands;
  presetCommands.push_back( str(format("/run/verbose %d") % m_VerboseLevel) );
  presetCommands.push_back( str(format("/event/verbose %d") % m_VerboseLevel) );
  presetCommands.push_back( str(format("/tracking/verbose %d") % m_VerboseLevel) );

  std::cout << "\nApplying preset commands:" << std::endl;
  for (const std::string& com: presetCommands) {
    std::cout << com << std::endl;
    ui->ApplyCommand(com);
  }

  std::cout << "\nApplying user commands:" << std::endl;
  for (const std::string& com: m_UserCommands) {
    std::cout << com << std::endl;
    ui->ApplyCommand(com);
  }
  std::cout << std::endl;
}

ANLStatus Geant4MTBody::mod_begin_run()
{
  return AS_OK;
}

ANLStatus Geant4MTBody::mod_analyze()
{
  if (m_EventIndex == 0) {
    m_G4RunManager->BeamOn(m_NEvents);
  }
  ++m_EventIndex;

  return AS_OK;
}

ANLStatus Geant4MTBody::mod_end_run()
{
  return AS_OK;
}

ANLStatus Geant4MTBody::mod_finalize()
{
  if (m_OutputRandomStatus) {
    CLHEP::HepRandom::saveEngineStatus(m_RandomFinalStatusFileName.c_str());
  }

  m_G4RunManager.reset(nullptr);

  return AS_OK;
}

} /* namespace anlgeant4 */
