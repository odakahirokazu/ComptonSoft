########################################################
set(TARGET_EXT_LIBRARY comptonSoft)
set(SWIG_IF_FILE ${TARGET_EXT_LIBRARY}.i)
set(CLASS_LIST_FILE class_list_${TARGET_EXT_LIBRARY}.hh)
set(RUBY_EXT_INCLUDE_DIRS
  ../source/tools/include
  ../source/modules/include
  ../source/simulation/include
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${CLHEP_INC_DIR}
  ${G4_INC_DIR}
  ${SIMX_INC_DIR}
  ${BOOST_INC_DIR}
  ${ANLG4_DIR}/ruby
  ${ANLNEXT_INC_DIR}/ruby
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
  CSSimulation
  CSModules
  CSTools
  ${ANLG4_LIB}
  ${ANLNEXT_LIB}
  ${ROOT_LIB}
  ${G4_LIB}
  ${SIMX_LIB}
  )

########################################################
set(cxx_definitions )
if(CS_USE_SIMX)
  list(APPEND cxx_definitions -DUSE_SIMX)
endif()
if(CS_USE_ROOT)
  list(APPEND cxx_definitions -DUSE_ROOT -DCS_BASIC2)
endif()

set(SWIG_FLAGS ${cxx_definitions})
add_definitions(${cxx_definitions})

########################################################
include(CreateSwigRuby)

install(FILES ${SWIG_IF_FILE} ${CLASS_LIST_FILE}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft/ruby)
