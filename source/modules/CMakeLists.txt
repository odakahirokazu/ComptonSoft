cmake_minimum_required(VERSION 3.0)

add_definitions(-DANLNEXT_USE_TVECTOR -DANLNEXT_USE_HEPVECTOR)

include_directories(
  include
  ../core/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${CLHEP_INC_DIR}
  ${CFITSIO_INC_DIR}
  ${SIMX_INC_DIR}
  ${ADD_INC_DIR}
  ${QT_INC_DIR}
  ${BOOST_INC_DIR}
  )

link_directories(
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${SIMX_LIB_DIR}
  )

set(CS_MODULES
  ### Basic modules
  src/ConstructDetector.cc
  src/ConstructDetectorForSimulation.cc
  src/VCSModule.cc
  src/CSHitCollection.cc
  src/ConstructChannelMap.cc
  src/SetNoiseLevels.cc
  src/SetBadChannels.cc
  src/SetChannelProperties.cc
  src/SelectHits.cc
  src/AnalyzeHits.cc
  src/MakeDetectorHits.cc
  src/MakeRawHits.cc
  src/MakeDetectorHitsWithTimingProcess.cc
  src/ApplyEPICompensation.cc
  src/EventSelection.cc
  src/EventReconstruction.cc
  src/HXIEventSelection.cc
  ### Frame data processes
  src/AssignTime.cc
  src/DefineFrame.cc
  src/ConstructFrame.cc
  src/FillFrame.cc
  src/LoadFrame.cc
  src/AnalyzeFrame.cc
  src/WriteXrayEventTree.cc
  src/SortEventTreeWithTime.cc
  ### IO
  src/CreateRootFile.cc
  src/SaveData.cc
  src/ReadDataFile.cc
  src/ReadDataFile_VME3.cc
  src/ReadDataFile_SpW2.cc
  src/ReadDataFile_NT.cc
  src/ReadDataFile_NB0.cc
  ### processing analysis
  src/CorrectPHA.cc
  src/RecalculateEPI.cc
  src/CalculatePedestalLevels.cc
  src/InitialConditionFilter.cc
  src/WeightByInitialDirection.cc
  src/RecalculateSimulationNoise.cc
  src/ComptonModeFilter.cc # for old SGD
  src/SelectFullDeposit.cc
  src/ComptonEventFilter.cc
  src/SelectEventsOnFocalPlane.cc
  src/SelectTime.cc
  src/FilterByGoodTimeIntervals.cc
  ### analysis ROOT trees & histograms
  src/WriteHitTree.cc
  src/ReadHitTree.cc
  src/ReadHitTreeAsRawHits.cc
  src/ReadHitTreeAsDetectorHits.cc
  src/WriteEventTree.cc
  src/ReadEventTree.cc
  src/ReadEventTreeAsRawHits.cc
  src/ReadEventTreeAsDetectorHits.cc
  src/WriteComptonEventTree.cc
  src/ReadComptonEventTree.cc
  src/HistogramPHA.cc
  src/HistogramEnergySpectrum.cc
  src/HistogramEnergy1D.cc
  src/HistogramEnergy2D.cc
  src/HistogramARM.cc
  src/HistogramARMByPositionMeasurement.cc
  src/HistogramAzimuthAngle.cc
  src/Histogram2DDeltaEnergyWithARM.cc
  src/ResponseMatrix.cc
  src/BackProjection.cc
  src/BackProjectionSky.cc
  src/EfficiencyMapSky.cc
  ### others
  src/QuickAnalysisForDSD.cc
  )

if(CS_USE_FITSIO)
  set(CS_FITSIO_MODULES
    ### Frame data processes
    src/MakeFrameFITS.cc
    )
endif()

add_library(CSModules SHARED
  ${CS_MODULES}
  ${CS_FITSIO_MODULES}
  )

target_link_libraries(CSModules
  CSCore ${ANLG4_LIB} ${ANLNEXT_LIB}
  ${ROOT_LIB} ${G4_LIB} ${CLHEP_LIB} ${CFITSIO_LIB} ${SIMX_LIB} ${ADD_LIB})

install(TARGETS CSModules LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
