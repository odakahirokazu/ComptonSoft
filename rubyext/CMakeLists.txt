########################################################
set(TARGET_EXT_LIBRARY comptonSoft)
set(EXT_LIB_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby/comptonsoft)
set(SWIG_IF_FILE ${TARGET_EXT_LIBRARY}.i)
set(CLASS_LIST_FILE class_list_${TARGET_EXT_LIBRARY}.hh)
set(RUBY_EXT_INCLUDE_DIRS
  ../source/core/include
  ../source/modules/include
  ../source/simulation/include
  ../source/astroh/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${CLHEP_INC_DIR}
  ${G4_INC_DIR}
  ${SIMX_INC_DIR}
  ${BOOST_INC_DIR}
  ${ANLG4_DIR}/rubyext
  .
  )
set(RUBY_EXT_LIBRARY_DIRS
  /usr/local/lib
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${G4_LIB_DIR}
  ${SIMX_LIB_DIR}
  ${ADD_LIB_DIR}
  )
set(RUBY_EXT_LIBRARIES
  CSAstroH
  CSSimulation
  CSModules
  CSCore
  ${ANLG4_LIB}
  ${ANLNEXT_LIB}
  ${G4_LIB}
  ${ROOT_LIB}
  ${SIMX_LIB}
  )

########################################################
set(cxx_definitions )
if(CS_USE_SIMX)
  list(APPEND cxx_definitions -DUSE_SIMX)
endif()
if(CS_USE_FITSIO)
  list(APPEND cxx_definitions -DUSE_FITSIO)
endif()

set(SWIG_FLAGS ${cxx_definitions})
add_definitions(${cxx_definitions})

########################################################
include(CreateSwigRuby)

install(FILES ${SWIG_IF_FILE} ${CLASS_LIST_FILE}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft/ruby)
