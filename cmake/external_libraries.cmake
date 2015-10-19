### BOOST ###
find_package(Boost 1.56.0 REQUIRED COMPONENTS system filesystem)
set(BOOST_INC_DIR ${Boost_INCLUDE_DIRS})
set(BOOST_LIB_DIR ${Boost_LIBRARY_DIRS})
set(BOOST_LIB ${Boost_LIBRARIES})
message("-- BOOST_INC_DIR: ${BOOST_INC_DIR}")
message("-- BOOST_LIB_DIR: ${BOOST_LIB_DIR}")
message("-- BOOST_LIB: ${BOOST_LIB}")

### ANL ###
if(NOT DEFINED ANLNEXT_INSTALL)
  if(DEFINED ENV{ANLNEXT_INSTALL})
    set(ANLNEXT_INSTALL $ENV{ANLNEXT_INSTALL})
  else()
    set(ANLNEXT_INSTALL $ENV{HOME})
  endif()
endif(NOT DEFINED ANLNEXT_INSTALL)
set(ANLNEXT_INC_DIR ${ANLNEXT_INSTALL}/include/anl)
set(ANLNEXT_LIB_DIR ${ANLNEXT_INSTALL}/lib)
set(ANLNEXT_LIB ANLNext)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ANLNEXT_LIB_DIR}/anl)
message("-- ANLNEXT_INSTALL = ${ANLNEXT_INSTALL}")

### ROOT ###
set(ROOTSYS $ENV{ROOTSYS})
find_path(ROOT_INC_DIR
  NAMES TH1.h TTree.h
  PATHS ${ROOTSYS}/include/root ${ROOTSYS}/include)
find_path(ROOT_LIB_DIR
  NAMES libHist.so libTree.so
  PATHS ${ROOTSYS}/lib/root ${ROOTSYS}/lib)
set(ROOT_LIB
  Core RIO Net Hist Graf Graf3d Gpad Tree
  Rint Postscript Matrix Physics MathCore Thread)
message("-- ROOTSYS = ${ROOTSYS}")
message("-- ROOT_INC_DIR = ${ROOT_INC_DIR}")
message("-- ROOT_LIB_DIR = ${ROOT_LIB_DIR}")
add_definitions(-DUSE_ROOT)
add_definitions(-DCS_BASIC2)

### Geant4 ###
find_package(Geant4 REQUIRED)
if(CS_USE_VIS)
  find_package(Geant4 COMPONENTS vis_opengl_x11 REQUIRED)
elseif(CS_USE_VIS_QT)
  find_package(Geant4 COMPONENTS vis_opengl_x11 qt REQUIRED)
endif()
# include(${Geant4_USE_FILE})
set(G4_INC_DIR ${Geant4_INCLUDE_DIRS})
set(G4_LIB ${Geant4_LIBRARIES})
message("-- Geant4 libraries: ${G4_LIB}")

### CLHEP ###
if(CS_USE_SYSTEM_CLHEP)
  add_definitions(-DANL_USE_HEPVECTOR)
  set(CLHEP_BASE_DIR $ENV{CLHEP_BASE_DIR})
  set(CLHEP_INC_DIR ${CLHEP_BASE_DIR}/include)
  set(CLHEP_LIB_DIR ${CLHEP_BASE_DIR}/lib)
  set(CLHEP_LIB CLHEP)
endif(CS_USE_SYSTEM_CLHEP)

### FITS IO ###
if(CS_USE_FITSIO)
  find_path(CFITSIO_INC_DIR
    NAMES fitsio.h
    PATHS /usr/local/include $ENV{HEADAS}/include)
  find_library(CFITSIO_LIB
    NAMES cfitsio
    PATHS /usr/local/lib $ENV{HEADAS}/lib)
endif(CS_USE_FITSIO)

### SIMX ###
if(CS_USE_SIMX)
  set(SIMX_TOP $ENV{SIMX})
  set(SIMX_INC_DIR
    ${SIMX_TOP}/include
    ${SIMX_TOP}/src
    )
  set(SIMX_LIB_DIR
    ${SIMX_TOP}/lib
    )
  set(SIMX_LIB simx simput cfitsio ape wcs fftw3 readline termcap)
endif(CS_USE_SIMX)
