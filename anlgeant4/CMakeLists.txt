set(TARGET_LIBRARY ANLGeant4)

if(ANLG4_USE_GDML)
  set(GDML_CLASSES
    src/ReadGDML.cc
    src/GDMLDetectorConstruction.cc
    src/WriteGDML.cc
    )
  set(GDML_INC_DIR ${XercesC_INC_DIRS})
  set(GDML_LIB ${XercesC_LIBRARIES})
endif(ANLG4_USE_GDML)

if(ANLG4_USE_VIS OR ANLG4_USE_VIS_QT)
  set(VIS_CLASSES
    src/VisualizeG4Geom.cc 
    )
  if(ANLG4_USE_VIS)
    add_definitions(-DANLG4_USE_VIS)
  endif()
  if(ANLG4_USE_VIS_QT)
    set(QT_INC_DIR
      /usr/local/include/QtCore
      /usr/local/include/QtDeclarative
      /usr/local/include/QtOpenGL
      /usr/local/include/QtGui
      )
    add_definitions(-DANLG4_USE_VIS_QT)
  endif()
endif()

add_definitions(-DANLNEXT_USE_HEPVECTOR)
 
include_directories(include
  ${CLHEP_INC_DIR}
  ${G4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${QT_INC_DIR}
  ${BOOST_INC_DIR}
  ${GDML_INC_DIR}
  )

link_directories(
  /usr/local/lib
  ${CLHEP_LIB_DIR}
  ${G4_LIB_DIR}
  ${ANLNEXT_LIB_DIR}
  )

add_library(${TARGET_LIBRARY} SHARED
  src/InitialInformation.cc
  src/ANLG4RunManager.cc
  src/Geant4Body.cc
  src/Geant4Simple.cc
  src/BasicPrimaryGeneratorAction.cc
  src/BasicPrimaryGen.cc
  src/PointSourcePrimaryGen.cc
  src/SphericalSourcePrimaryGen.cc
  src/PlaneWavePrimaryGen.cc
  src/PlaneWaveRectanglePrimaryGen.cc
  src/GaussianBeamPrimaryGen.cc
  src/IsotropicPrimaryGen.cc
  src/PrimaryGenUniformSourceInVolume.cc
  src/PositionSamplerInVolume.cc
  src/NucleusPrimaryGen.cc
  src/NucleusPrimaryGenInVolume.cc
  src/UserActionAssemblyRunAction.cc
  src/UserActionAssemblyEventAction.cc
  src/UserActionAssemblyTrackingAction.cc
  src/UserActionAssemblySteppingAction.cc
  src/VUserActionAssembly.cc
  src/VMasterUserActionAssembly.cc
  src/VAppendableUserActionAssembly.cc
  src/StandardUserActionAssembly.cc
  src/VANLGeometry.cc
  ${GDML_CLASSES}
  ${VIS_CLASSES}
  )

target_link_libraries(${TARGET_LIBRARY}
  ${ANLNEXT_LIB} ${G4_LIB} ${CLHEP_LIB} ${GDML_LIB})

install(TARGETS ${TARGET_LIBRARY}
  LIBRARY
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/anlgeant4
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)

if(USE_RUBY)
  add_subdirectory(rubyext)
endif(USE_RUBY)
