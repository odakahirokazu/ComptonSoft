### BOOST ###
find_package(Boost 1.56.0 REQUIRED COMPONENTS system filesystem)
set(BOOST_INC_DIR ${Boost_INCLUDE_DIRS})
set(BOOST_LIB_DIR ${Boost_LIBRARY_DIRS})
set(BOOST_LIB ${Boost_LIBRARIES})
message("-- BOOST_INC_DIR: ${BOOST_INC_DIR}")
message("-- BOOST_LIB_DIR: ${BOOST_LIB_DIR}")
message("-- BOOST_LIB: ${BOOST_LIB}")

### Workaround for Clang-15 not to use std::unary_function
add_compile_definitions(_HAS_AUTO_PTR_ETC=FALSE)

### ANL ###
if(NOT DEFINED ANLNEXT_INSTALL)
  if(DEFINED ENV{ANLNEXT_INSTALL})
    set(ANLNEXT_INSTALL $ENV{ANLNEXT_INSTALL})
  else()
    set(ANLNEXT_INSTALL $ENV{HOME})
  endif()
endif(NOT DEFINED ANLNEXT_INSTALL)
set(ANLNEXT_INC_DIR ${ANLNEXT_INSTALL}/include)
set(ANLNEXT_LIB_DIR ${ANLNEXT_INSTALL}/lib)
set(ANLNEXT_LIB ANLNext)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ANLNEXT_LIB_DIR}/anlnext)
message("-- ANLNEXT_INSTALL = ${ANLNEXT_INSTALL}")

### ROOT ###
set(ROOTSYS $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH ${ROOTSYS})
find_package(ROOT REQUIRED)
set(ROOT_INC_DIR ${ROOT_INCLUDE_DIRS})
set(ROOT_LIB_DIR ${ROOT_LIBRARY_DIR})
set(ROOT_LIB ${ROOT_LIBRARIES})
list(APPEND CMAKE_INSTALL_RPATH ${ROOT_LIBRARY_DIR})
message("-- ROOTSYS = ${ROOTSYS}")
message("-- ROOT_INC_DIR = ${ROOT_INC_DIR}")
message("-- ROOT_LIB_DIR = ${ROOT_LIB_DIR}")
message("-- ROOT libraries = ${ROOT_LIB}")
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
  add_definitions(-DANLNEXT_USE_HEPVECTOR)
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

### HEALPIX ###
if(CS_USE_HEALPIX)
  find_path(HEALPIX_INC_DIR
    NAMES healpix_map.h
    PATHS /usr/local/include/healpix_cxx /opt/homebrew/include/healpix_cxx)
  find_library(HEALPIX_LIB
    NAMES healpix_cxx
    PATHS /usr/local/lib /opt/homebrew/lib)
endif(CS_USE_HEALPIX)

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

### HSQuickLook ###
if(CS_USE_HSQUICKLOOK)
  find_path(HSQUICKLOOK_INC_DIR
    NAMES hsquicklook/DocumentBuilder.hh
    PATHS $ENV{HSQUICKLOOK_INSTALL}/include /usr/local/include $ENV{HOME}/include)
  find_library(HSQUICKLOOK_LIB
    NAMES HSQuickLookAnalyzer
    PATHS $ENV{HSQUICKLOOK_INSTALL}/lib /usr/local/lib $ENV{HOME}/lib)
  message("-- HSQUICKLOOK_INC_DIR: ${HSQUICKLOOK_INC_DIR}")
  message("-- HSQUICKLOOK_LIB: ${HSQUICKLOOK_LIB}")

  find_package(mongocxx REQUIRED)
  set(MONGOCXX_LIB mongo::mongocxx_shared)
endif(CS_USE_HSQUICKLOOK)

### Xerces-C ###
if(CS_USE_GDML)
  find_package(XercesC REQUIRED)
  message("-- XercesC_INCLUDE_DIRS: ${XercesC_INCLUDE_DIRS}")
  message("-- XercesC_LIBRARIES: ${XercesC_LIBRARIES}")
endif(CS_USE_GDML)
