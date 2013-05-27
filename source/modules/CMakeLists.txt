cmake_minimum_required(VERSION 2.8)

if(CS_USE_ROOT)
  add_definitions(-DANL_USE_TVECTOR -DANL_USE_HEPVECTOR)
else()
  add_definitions(-DANL_USE_HEPVECTOR)
endif()

include_directories(
  include
  ../tools/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${CLHEP_INC_DIR}
  ${SIMX_INC_DIR}
  ${ADD_INC_DIR}
  ${QT_INC_DIR}
  ${BOOST_INC_DIR}
  )

link_directories(
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${SIMX_LIB_DIR}
  )

set(CS_MODULES_BASIC1
  src/VCSModule.cc
  src/CSHitCollection.cc
  src/ConstructDetector.cc
  src/ConstructDetector_Sim.cc
  src/ConstructChannelTable.cc
  src/DefineBadChannel.cc
  src/DefineBadChannel_Sim.cc
  src/SetNoiseLevel.cc
  src/SelectHit.cc
  src/AnalyzeHit.cc
  src/MakeDetectorHit.cc
  src/MakeRawHit.cc
  src/MakeDetectorHitTR.cc
  src/EventSelection.cc
  src/EventReconstruction.cc
  src/HXIEventSelection.cc
)

if(CS_USE_ROOT)
  set(CS_MODULES_BASIC2
    # IO
    src/SaveData.cc
    src/ReadDataFile.cc
    src/ReadDataFile_VME3.cc
    src/ReadDataFile_SpW2.cc
    src/ReadDataFile_NT.cc
    src/ReadDataFile_NB0.cc
    # processing analysis
    src/CorrectPHA.cc
    src/MakePI.cc
    src/ApplyNewPI.cc
    src/CalcPedestal.cc
    src/CalcPedestal_SpW.cc
    src/InitialConditionFilter.cc
    src/ReprocessPI.cc
    src/EventThreshold.cc
    src/Clustering.cc
    src/SelectHitPattern.cc
    src/ComptonModeFilter.cc
    src/FilterFullDeposit.cc
    src/ComptonEventFilter.cc
    src/SelectTime.cc
    # analysis ROOT histograms & trees
    src/HitTree.cc
    src/HitTree_Sim.cc
    src/EventTree.cc
    src/RawTree.cc
    src/RawTree_SpW.cc
    src/HistPHA.cc
    src/HistPI.cc
    src/DeadTimeTree_VME3.cc
    src/ReadHitTree.cc
    src/ReadRawHitTree.cc
    src/ReadDetectorHitTree.cc
    src/PhAbsModeSpectrum.cc
    src/ComptonModeSpectrum.cc
    src/Edep2DMap.cc
    src/Resp2DSpec.cc
    src/EnergySpectrum.cc
    src/ResponseMatrix.cc
    src/CalcARM.cc
    src/CalcARMEne.cc
    src/CalcARMPos.cc
    src/CalcARM_PS.cc
    src/ComptonTree.cc
    src/ReadComptonTree.cc
    src/VAnalyzeHitList.cc
    src/BackProjection.cc
    src/BackProjectionSky.cc
    src/AzimuthDistribution.cc
    src/AnalyzeDSD.cc
    # others
    src/SetSimGainCorrection.cc
    src/GainCorrection.cc
    src/EfficiencyMapSky.cc
    )
endif(CS_USE_ROOT)

add_library(CSModules SHARED
  ${CS_MODULES_BASIC1}
  ${CS_MODULES_BASIC2}
  )

target_link_libraries(CSModules
  CSTools ${ANLG4_LIB} ${ANLNEXT_LIB}
  ${ROOT_LIB} ${G4_LIB} ${CLHEP_LIB} ${SIMX_LIB} ${ADD_LIB})

install(TARGETS CSModules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
