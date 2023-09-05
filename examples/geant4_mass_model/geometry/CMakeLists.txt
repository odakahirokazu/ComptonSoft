cmake_minimum_required(VERSION 3.20)

### Initial definition of cmake variables
set(CMAKE_INSTALL_PREFIX $ENV{HOME} CACHE PATH "install prefix")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "build type")
set(CMAKE_CXX_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "CXX_FLAGS for debug")
set(CMAKE_C_FLAGS_DEBUG "-g -W -Wall" CACHE STRING "C_FLAGS for debug")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "CXX_FLAGS for release")
set(CMAKE_C_FLAGS_RELEASE "-O3 -W -Wall" CACHE STRING "C_FLAGS for release")
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

### Definition of project
set(GEOMETRY_LIB MyGeometry)

project(${GEOMETRY_LIB})
set(GEOMETRY_CXX_LIB ${GEOMETRY_LIB}_CS)
set(CMAKE_CXX_STANDARD 14)

### External software ###
### ComptonSoft ###
if(NOT DEFINED CS_INSTALL)
  if(DEFINED ENV{CS_INSTALL})
    set(CS_INSTALL $ENV{CS_INSTALL})
  else()
    set(CS_INSTALL $ENV{HOME})
  endif()
endif(NOT DEFINED CS_INSTALL)

set(CMAKE_MODULE_PATH ${CS_INSTALL}/lib/comptonsoft ${CMAKE_MODULE_PATH})
include(UseComptonSoft)

message("-- Install destination prefix: ${CMAKE_INSTALL_PREFIX}")

### Main ###
set(TARGET_LIBRARY ${GEOMETRY_CXX_LIB})

option(USE_RUBY "enable Ruby binding" ON)
option(CS_USE_VIS_QT "enable Geant4 visualization based on QT" ON)

add_definitions(-DANL_USE_HEPVECTOR)
include_directories(include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${G4_INC_DIR}
  )

link_directories(
  ${ANLG4_LIB_DIR}
  ${ANLNEXT_LIB_DIR}
  ${G4_LIB_DIR}
  )

add_library(${TARGET_LIBRARY} SHARED
  src/MyMainGeometry.cc
  src/MyDetectorConstruction.cc
  )

target_link_libraries(${TARGET_LIBRARY}
  ${ANLG4_LIB}
  ${ANLNEXT_LIB}
  ${G4_LIB}
  )

install(TARGETS ${TARGET_LIBRARY}
  LIBRARY
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(USE_RUBY)
  add_subdirectory(rubyext)
endif(USE_RUBY)
