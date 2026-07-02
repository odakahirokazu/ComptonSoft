########################################################
include(ANLNextSwigPython)

set(target_binding anlGeant4Py)
set(swig_if_file interface/anlGeant4Py.i)
set(class_list_file interface/class_list_anlGeant4Py.hh)

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
anlnext_add_swig_python_binding(${target_binding}
  INTERFACE_FILE ${swig_if_file}
  INCLUDE_DIRECTORIES ${ANLNEXT_INC_DIR} interface
  LIBRARIES ANLGeant4
  SWIG_FLAGS ${swig_flags}
  COMPILE_DEFINITIONS ${cxx_definitions}
  INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python/anlgeant4
)

########################################################
install(FILES ${swig_if_file} ${class_list_file}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/anlgeant4/python)

install(DIRECTORY library/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python
  FILES_MATCHING PATTERN "*.py")
########################################################
