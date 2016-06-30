cmake_minimum_required(VERSION 3.0)

add_definitions(-DANL_USE_TVECTOR -DANL_USE_HEPVECTOR)

include_directories(
  include
  ../core/include
  ../modules/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${CLHEP_INC_DIR}
  ${ADD_INC_DIR}
  ${SIMX_INC_DIR}
  ${CFITSIO_INC_DIR}
  ${BOOST_INC_DIR}
  )

link_directories(
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${SIMX_LIB_DIR}
  ${CFITSIO_LIB_DIR}
  )

set(CS_SIMULATION_CLASSES_BASIC
  # primary geneator
  src/AHRadiationBackgroundPrimaryGen.cc
  # pick up data
  src/RadioactiveDecayPickUpData.cc
  src/ActivationStackingAction.cc
  src/ActivationPickUpData.cc
  src/KillStepPickUpData.cc
  src/MaterialSamplePickUpData.cc
  src/ScatteringPickUpData.cc
  # physics list
  src/AHG4EmLivermorePhysics.cc
  src/CSPhysicsList.cc
  src/PhysicsListManager.cc
  # others
  src/AssignG4CopyNumber.cc
  src/DumpMass.cc
  src/InitialParticleTree.cc
  )

if(CS_USE_SIMX)
  set(CS_SIMULATION_CLASSES_SIMX
    # basic interface
    src/SimXIF.cc
    # primary generator
    src/SimXPrimaryGen.cc  
    src/AHRayTracingPrimaryGen.cc
    # pick up data
    src/AHStandardPickUpData.cc
    # other
    src/OutputSimXPrimaries.cc
    src/GenerateSimXEvent.cc
    )
endif(CS_USE_SIMX)

add_library(CSSimulation SHARED
  ${CS_SIMULATION_CLASSES_BASIC}
  ${CS_SIMULATION_CLASSES_SIMX}
  )

target_link_libraries(CSSimulation
  CSCore CSModules ${ANLG4_LIB} ${ANLNEXT_LIB}
  ${ROOT_LIB} ${G4_LIB} ${CLHEP_LIB} ${ADD_LIB} ${SIMX_LIB} ${CFITSIO_LIB})

install(TARGETS CSSimulation LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
