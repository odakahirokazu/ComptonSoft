########################################################
set(TARGET_EXT_LIBRARY anlGeant4Py)
set(EXT_LIB_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python/anlgeant4)
set(SWIG_IF_FILE interface/${TARGET_EXT_LIBRARY}.i)
set(CLASS_LIST_FILE interface/class_list_${TARGET_EXT_LIBRARY}.hh)
set(PYTHON_EXT_INCLUDE_DIRS
  ../include
  ${ANLNEXT_INC_DIR}
  ${BOOST_INC_DIR}
)
set(PYTHON_EXT_LIBRARY_DIRS ${ANLNEXT_LIB_DIR})
set(PYTHON_EXT_LIBRARIES ANLGeant4 ${ANLNEXT_LIB})

########################################################
set(cxx_definitions )
if(ANLG4_USE_GDML)
  list(APPEND cxx_definitions -DUSE_GDML)
endif()
if(ANLG4_USE_VIS OR ANLG4_USE_VIS_QT)
  list(APPEND cxx_definitions -DUSE_VIS)
endif()

set(SWIG_FLAGS ${cxx_definitions})
add_definitions(${cxx_definitions})

########################################################
include(CreateSwigPython)

install(FILES ${SWIG_IF_FILE} ${CLASS_LIST_FILE}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/anlgeant4/python)

########################################################
# install Python libraries written in Python
########################################################
install(DIRECTORY library/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python
  FILES_MATCHING PATTERN "*.py")
