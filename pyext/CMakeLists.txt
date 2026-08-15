########################################################
set(TARGET_EXT_LIBRARY comptonSoftPy)
set(SWIG_IF_FILE interface/${TARGET_EXT_LIBRARY}.i)

set(PYTHON_EXT_INCLUDE_DIRS
  ${ComptonSoft_SOURCE_DIR}/source/core/include
  ${ComptonSoft_SOURCE_DIR}/source/core/include/nanograms
  ${ComptonSoft_SOURCE_DIR}/source/modules/include
  ${ComptonSoft_SOURCE_DIR}/source/simulation/include
  ${ComptonSoft_SOURCE_DIR}/source/astroh/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${CLHEP_INC_DIR}
  ${G4_INC_DIR}
  ${BOOST_INC_DIR}
)

# ANLNext is linked by its library name rather than as an imported CMake
# target in ComptonSoft's existing build configuration.
link_directories(${ANLNEXT_LIB_DIR})

set(cxx_definitions)
if(CS_USE_SIMX)
  list(APPEND cxx_definitions -DUSE_SIMX)
endif()
if(CS_USE_FITSIO)
  list(APPEND cxx_definitions -DUSE_FITSIO)
endif()
if(CS_USE_HEALPIX)
  list(APPEND cxx_definitions -DUSE_HEALPIX)
endif()
if(CS_USE_HSQUICKLOOK)
  list(APPEND cxx_definitions -DUSE_HSQUICKLOOK)
endif()

include(ANLNextSwigPython)
anlnext_add_swig_python_binding(${TARGET_EXT_LIBRARY}
  INTERFACE_FILE ${SWIG_IF_FILE}
  INCLUDE_DIRECTORIES ${PYTHON_EXT_INCLUDE_DIRS}
  LIBRARIES
    CSAstroH
    CSSimulation
    CSModules
    CSCore
    ${ANLG4_LIB}
    ${ANLNEXT_LIB}
    ${ROOT_LIB}
    ${G4_LIB}
    ${CLHEP_LIB}
  COMPILE_DEFINITIONS ${cxx_definitions}
  SWIG_FLAGS ${cxx_definitions}
  INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python/comptonsoft
)

install(FILES ${SWIG_IF_FILE}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft/python)
install(DIRECTORY library/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python
  FILES_MATCHING PATTERN "*.py")
