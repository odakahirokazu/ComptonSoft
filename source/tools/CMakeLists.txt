cmake_minimum_required(VERSION 2.8)

include_directories(include
  ${ANLG4_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${CLHEP_INC_DIR}
  ${ADD_INC_DIR}
  ${BOOST_INC_DIR}
  )

link_directories(
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${BOOST_LIB_DIR}
  )

set(CS_TOOLS_CLASSES_BASIC1
  # detector
  src/DetectorHit.cc
  src/OneASICData.cc
  src/VDetectorUnit.cc
  src/RealDetectorUnit.cc
  src/RealDetectorUnit2DPad.cc
  src/RealDetectorUnit2DStrip.cc
  src/RealDetectorUnitScintillator.cc
  src/VDeviceSimulation.cc
  src/BasicDeviceSimulation.cc
  src/SimDetectorUnitLite.cc
  # Compton camera
  src/TwoHitComptonEvent.cc
  src/ComptonEvent.cc
  src/DetectorGroup.cc
  src/HitPattern.cc
  # manager
  src/DetectorManager.cc
)

if(CS_USE_ROOT)
  set(CS_TOOLS_CLASSES_BASIC2
    src/OneASICDataGainCal.cc
    src/DeviceSimulation.cc
    src/CalcWPPixel.cc
    src/CalcWPStrip.cc
    src/EFieldModel.cc
    src/SimDetectorUnit2DPad.cc
    src/SimDetectorUnit2DStrip.cc
    src/SimDetectorUnitScintillator.cc
    )
endif(CS_USE_ROOT)

set(CS_TOOLS_CLASSES_G4SD
  # sensitive detector
  src/VHXISGDSensitiveDetector.cc
  src/HXISGDSensitiveDetector.cc
  src/AHSensitiveDetector.cc
)

add_library(CSTools SHARED 
  ${CS_TOOLS_CLASSES_BASIC1}
  ${CS_TOOLS_CLASSES_BASIC2}
  ${CS_TOOLS_CLASSES_G4SD}
  )

target_link_libraries(CSTools
  ${ROOT_LIB} ${G4_LIB} ${CLHEP_LIB} ${ADD_LIB} ${BOOST_LIB})

install(TARGETS CSTools LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
