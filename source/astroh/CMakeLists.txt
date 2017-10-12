cmake_minimum_required(VERSION 3.0)

add_definitions(-DANLNEXT_USE_TVECTOR -DANLNEXT_USE_HEPVECTOR)

include_directories(
  include
  ../core/include
  ../modules/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${CLHEP_INC_DIR}
  ${SIMX_INC_DIR}
  ${CFITSIO_INC_DIR}
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

set(CS_ASTROH_CLASSES
  src/SGDEventTree.cc
  ### Basic modules
  # src/ReadHXIEventTree.cc
  src/ReadSGDEventTree.cc
  src/SimulateCXBShieldPlate.cc
  src/RescaleSimulationNoiseOfSGDSiUntriggered.cc
  src/UniformlyRandomizeEPI.cc
  )

if(CS_USE_FITSIO)
  set(CS_ASTROH_FITSIO_CLASSES
    src/SGDEventFITS.cc
    src/ReadSGDEventFITS.cc
    src/WriteSGDEventFITS.cc
    src/HXIEventFITS.cc
    src/ReadHXIEventFITS.cc
    src/WriteHXIEventFITS.cc
    src/FilterByGoodTimeIntervalsForSGD.cc
    src/FilterByGoodTimeIntervalsForHXI.cc
    )
endif()

add_library(CSAstroH SHARED
  ${CS_ASTROH_CLASSES}
  ${CS_ASTROH_FITSIO_CLASSES}
  )

target_link_libraries(CSAstroH
  CSCore CSModules ${ANLG4_LIB} ${ANLNEXT_LIB}
  ${ROOT_LIB} ${G4_LIB} ${CLHEP_LIB} ${CFITSIO_LIB} ${SIMX_LIB} ${ADD_LIB})

install(TARGETS CSAstroH LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
