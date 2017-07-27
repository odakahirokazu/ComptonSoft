########################################################
set(TARGET_EXT_LIBRARY anlGeant4)
set(EXT_LIB_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby/anlgeant4)
set(SWIG_IF_FILE ${TARGET_EXT_LIBRARY}.i)
set(CLASS_LIST_FILE class_list_${TARGET_EXT_LIBRARY}.hh)
set(RUBY_EXT_INCLUDE_DIRS
  ../include
  ${ANLNEXT_INC_DIR}
  ${BOOST_INC_DIR}
)
set(RUBY_EXT_LIBRARY_DIRS ${ANLNEXT_LIB_DIR})
set(RUBY_EXT_LIBRARIES ANLGeant4 ${ANLNEXT_LIB})

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
include(CreateSwigRuby)

install(FILES ${SWIG_IF_FILE} ${CLASS_LIST_FILE}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/anlgeant4/ruby)

########################################################
# install Ruby libraries written in Ruby
########################################################
install(FILES anlgeant4.rb
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby)
