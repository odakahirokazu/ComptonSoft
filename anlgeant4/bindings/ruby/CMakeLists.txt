########################################################
include(ANLNextSwigRuby)

set(target_binding anlGeant4)
set(swig_if_file interface/anlGeant4.i)
set(class_list_file interface/class_list_anlGeant4.hh)

########################################################
set(cxx_definitions )
set(swig_flags )
if(ANLG4_USE_GDML)
  list(APPEND cxx_definitions USE_GDML)
  list(APPEND swig_flags -DUSE_GDML)
endif()
if(ANLG4_USE_VIS OR ANLG4_USE_VIS_QT)
  list(APPEND cxx_definitions USE_VIS)
  list(APPEND swig_flags -DUSE_VIS)
endif()

########################################################
anlnext_add_swig_ruby_binding(${target_binding}
  INTERFACE_FILE ${swig_if_file}
  INCLUDE_DIRECTORIES ${ANLNEXT_INC_DIR} interface
  LIBRARIES ANLGeant4
  SWIG_FLAGS ${swig_flags}
  COMPILE_DEFINITIONS ${cxx_definitions}
  INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby/anlgeant4
)

########################################################
install(FILES ${swig_if_file} ${class_list_file}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/anlgeant4/ruby)

install(DIRECTORY library/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby
  FILES_MATCHING PATTERN "*.rb")
########################################################
