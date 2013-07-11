cmake_minimum_required(VERSION 2.8)

if(CS_USE_ROOT)
  add_definitions(-DANL_USE_TVECTOR -DANL_USE_HEPVECTOR)
else()
  add_definitions(-DANL_USE_HEPVECTOR)
endif()

include_directories(
  include
  ../tools/include
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
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${SIMX_LIB_DIR}
  ${CFITSIO_LIB_DIR}
  )

set(CS_SIMULATION_CLASSES_BASIC1
  # primary geneator
  src/PlaneWavePrimaryGen.cc
  src/PlaneWaveRectanglePrimaryGen.cc
  src/PointSourcePrimaryGen.cc
  src/IsotropicPrimaryGen.cc
  src/NucleusPrimaryGen.cc
  src/ReadSPENVIS.cc
  src/SpenvisIsotropicPrimaryGen.cc
  # pick up data
  src/RDPickUpData.cc
  src/ActivationStackingAction.cc
  src/ActivationPickUpData.cc
  # physics list
  src/AHG4EmLivermorePhysics.cc
  src/AHG4RadioactiveDecayConstructor.cc
  src/AHG4PhysicsList.cc
  src/AHStandardANLPhysicsList.cc
  # others
  src/AssignG4CopyNumber.cc
  src/DumpMass.cc
  )

if(CS_USE_ROOT)
  set(CS_SIMULATION_CLASSES_BASIC2
    # primary geneator
    src/PlaneWavePrimaryGenPol.cc
    src/PlaneWaveRectanglePrimaryGenPol.cc
    src/VolumeInsideSample.cc
    src/PrimaryGenInVolume.cc
    src/NucleusPrimaryGenInVolume.cc
    src/AHRadiationBackgroundPrimaryGen.cc
    # pick up data
    src/KillStepPickUpData.cc
    src/MaterialSamplePickUpData.cc
    src/ScatteringPickUpData.cc
    # others
    src/InitialParticleTree.cc
    )
endif(CS_USE_ROOT)

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
  ${CS_SIMULATION_CLASSES_BASIC1}
  ${CS_SIMULATION_CLASSES_BASIC2}
  ${CS_SIMULATION_CLASSES_SIMX}
  )

target_link_libraries(CSSimulation
  CSTools CSModules ${ANLG4_LIB} ${ANLNEXT_LIB}
  ${ROOT_LIB} ${G4_LIB} ${CLHEP_LIB} ${ADD_LIB} ${SIMX_LIB} ${CFITSIO_LIB})

install(TARGETS CSSimulation LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
