add_library(ComptonSoftCore)

set(ComptonSoftCore_Base_Classes
  ### basic types and classes
  base/src/CSException.cc
  base/src/ChannelID.cc
  base/src/VoxelID.cc
  base/src/DetectorGroup.cc
  base/src/HitPattern.cc
  base/src/PhaseSpaceVector.cc
  base/src/IsotopeInfo.cc
  base/src/DetectorHit.cc
  base/src/BasicComptonEvent.cc
)

set(ComptonSoftCore_Framework_Classes
  ### processing/readout
  framework/src/VGainFunction.cc
  framework/src/GainFunctionLinear.cc
  framework/src/GainFunctionCubic.cc
  framework/src/GainFunctionSpline.cc
  framework/src/MultiChannelData.cc
  framework/src/VChannelMap.cc
  framework/src/ChannelMap.cc
  framework/src/ChannelMapDSD.cc
  framework/src/ReadoutModule.cc
  ### detector units
  framework/src/VDetectorUnit.cc
  framework/src/VRealDetectorUnit.cc
  framework/src/RealDetectorUnit2DPixel.cc
  framework/src/RealDetectorUnit2DStrip.cc
  framework/src/RealDetectorUnitScintillator.cc
  framework/src/RealDetectorUnit3DVoxel.cc
  ### frame readout
  framework/src/FrameData.cc
  framework/src/OutlierStore.cc
  framework/src/XrayEvent.cc
  framework/src/PolarizedXrayEvent.cc
  ### device simulation
  framework/src/EFieldModel.cc
  framework/src/VDeviceSimulation.cc
  framework/src/DeviceSimulation.cc
  framework/src/SimDetectorUnit2DPixel.cc
  framework/src/SimDetectorUnit2DStrip.cc
  framework/src/SimDetectorUnitScintillator.cc
  framework/src/SimDetectorUnit3DVoxel.cc
  framework/src/WeightingPotentialPixel.cc
  framework/src/WeightingPotentialStrip.cc
  ### detector unit factory
  framework/src/VDetectorUnitFactory.cc
  framework/src/RealDetectorUnitFactory.cc
  framework/src/SimDetectorUnitFactory.cc
  ### sensitive detector
  framework/src/VCSSensitiveDetector.cc
  ### manager
  framework/src/DetectorSystem.cc
)

set(ComptonSoftCore_Framework_Modules
  ### event store
  framework/src/CSEventStore.cc
  ### base and IO
  framework/src/VCSModule.cc
  framework/src/CSHitCollection.cc
  framework/src/CreateRootFile.cc
  framework/src/SaveData.cc
  ### detector construction
  framework/src/ConstructDetector.cc
  framework/src/ConstructDetectorForSimulation.cc
  ### detector processing
  framework/src/ConstructChannelMap.cc
  framework/src/SetNoiseLevels.cc
  framework/src/SetBadChannels.cc
  framework/src/SetChannelProperties.cc
  framework/src/SelectHits.cc
  framework/src/AnalyzeHits.cc
  framework/src/MakeDetectorHits.cc
  framework/src/MakeRawHits.cc
  framework/src/MakeDetectorHitsWithTimingProcess.cc
  framework/src/ApplyEPICompensation.cc
  framework/src/EventSelection.cc
)

set(ComptonSoftCore_Simulation_Classes
  ### physics list
  simulation/src/CustomizedEmLivermorePhysics.cc
  simulation/src/CustomizedEmLivermorePolarizedPhysics.cc
  simulation/src/CSPhysicsList.cc
  ### observation simulation
  simulation/src/ObservationPickUpData.cc
  simulation/src/ObservationTreeIO.cc
  simulation/src/ObservationTreeIOWithInitialInfo.cc
)

set(ComptonSoftCore_Simulation_Modules
  ### physics
  simulation/src/PhysicsListManager.cc
  ### primary generator
  simulation/src/ListPrimaryGenerator.cc
  ### observation simulation
  simulation/src/WriteObservationTree.cc
  ### utility
  simulation/src/InitialParticleTree.cc
)

if(CS_USE_HEALPIX)
  set(ComptonSoftCore_Simulation_Modules
    ${ComptonSoftCore_Simulation_Modules}
    simulation/src/AllSkyPrimaryGenerator.cc
  )
endif()

set(ComptonSoftCore_IO_Classes
  ### tree IO
  io/src/HitTreeIO.cc
  io/src/EventTreeIO.cc
  io/src/InitialInfoTreeIO.cc
  io/src/HitTreeIOWithInitialInfo.cc
  io/src/EventTreeIOWithInitialInfo.cc
)

set(ComptonSoftCore_IO_Modules
  io/src/WriteHitTree.cc
  io/src/ReadHitTree.cc
  io/src/ReadHitTreeAsRawHits.cc
  io/src/ReadHitTreeAsDetectorHits.cc
  io/src/WriteEventTree.cc
  io/src/ReadEventTree.cc
  io/src/ReadEventTreeAsRawHits.cc
  io/src/ReadEventTreeAsDetectorHits.cc
)

target_sources(ComptonSoftCore
  PRIVATE
  ${ComptonSoftCore_Base_Classes}
  ${ComptonSoftCore_Framework_Classes}
  ${ComptonSoftCore_Framework_Modules}
  ${ComptonSoftCore_Simulation_Classes}
  ${ComptonSoftCore_Simulation_Modules}
  ${ComptonSoftCore_IO_Classes}
  ${ComptonSoftCore_IO_Modules}
)

target_include_directories(ComptonSoftCore
  PUBLIC
  ${BOOST_INC_DIR}
  ${CLHEP_INC_DIR}
  ${G4_INC_DIR}
  ${ROOT_INC_DIR}
  ${HEALPIX_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ANLG4_INC_DIR}
  base/include
  framework/include
  simulation/include
  io/include
)

target_compile_definitions(ComptonSoftCore
  PUBLIC
  ANLNEXT_USE_TVECTOR
  ANLNEXT_USE_HEPVECTOR
)

target_link_directories(ComptonSoftCore
  PUBLIC
  ${ANLNEXT_LIB_DIR}
)

target_link_libraries(ComptonSoftCore
  PUBLIC
  ${BOOST_LIB}
  ${CLHEP_LIB}
  ${G4_LIB}
  ${ROOT_LIB}
  ${HEALPIX_LIB}
  ${ANLNEXT_LIB}
  ${ANLG4_LIB}
)

install(TARGETS ComptonSoftCore
  LIBRARY
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
