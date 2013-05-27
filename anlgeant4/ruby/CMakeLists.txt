########################################################
set(TARGET_EXT_LIBRARY anlGeant4)
set(SWIG_IF_FILE ${TARGET_EXT_LIBRARY}.i)
set(CLASS_LIST_FILE class_list_${TARGET_EXT_LIBRARY}.hh)
########################################################

# compile option (Debug) 
set(CMAKE_CXX_FLAGS_DEBUG "-W -g")
set(CMAKE_C_FLAGS_DEBUG "-W -g")

# compile option (Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_RELEASE "-O2")

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(Ruby)
set(RUBY_LINK_FLAGS " ")
if(${RUBY_VERSION} VERSION_GREATER 1.9.0)
  set(RUBY_INCLUDE_DIRS ${RUBY_HDR_DIR})
  set(RUBY_LIBRARY "")
  if(APPLE)
    set(RUBY_LINK_FLAGS "-Wl,-undefined,dynamic_lookup -Wl,-multiply_defined,suppress -Wl,-flat_namespace")
  endif()
endif()
message("-- Ruby include: ${RUBY_INCLUDE_DIRS}")
message("-- Ruby library: ${RUBY_LIBRARY}")

include_directories(
  ../include
  ${ANLNEXT_INC_DIR}
  ${BOOST_INC_DIR}
  ${RUBY_INCLUDE_DIRS}
  ${ANLNEXT_INC_DIR}/ruby
)

set(CMAKE_SWIG_FLAGS "")

link_directories(${ANLNEXT_LIB_DIR})

set(sflags "")
if(ANLG4_USE_GDML)
  set(sflags ${sflags} -DUSE_GDML)
endif(ANLG4_USE_GDML)

if(ANLG4_USE_VIS OR ANLG4_USE_VIS_QT)
  set(sflags ${sflags} -DUSE_VIS)
endif()

add_definitions(${sflags})
# set(sflags ${sflags} -includeall)
set_source_files_properties(${SWIG_IF_FILE} PROPERTIES CPLUSPLUS ON)
set_source_files_properties(${SWIG_IF_FILE} PROPERTIES SWIG_FLAGS "${sflags}")
add_definitions(-DHAVE_STRUCT_TIMESPEC -DHAVE_STRUCT_TIMEZONE -DHAVE_SIGNBIT)

swig_add_module(${TARGET_EXT_LIBRARY} ruby ${SWIG_IF_FILE})
swig_link_libraries(${TARGET_EXT_LIBRARY}
  ${RUBY_LIBRARY} ANLGeant4 ${ANLNEXT_LIB})

if(APPLE)
  set(RUBY_BINDING_SUFFIX ".bundle")
else(APPLE)
  set(RUBY_BINDING_SUFFIX ".so")
endif(APPLE)

set_target_properties(${TARGET_EXT_LIBRARY}
  PROPERTIES
  PREFIX ""
  SUFFIX ${RUBY_BINDING_SUFFIX}
  LINK_FLAGS ${RUBY_LINK_FLAGS}
  )

install(TARGETS ${TARGET_EXT_LIBRARY}
  LIBRARY
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby)

install(FILES ${SWIG_IF_FILE} ${CLASS_LIST_FILE}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/anlgeant4/ruby)
