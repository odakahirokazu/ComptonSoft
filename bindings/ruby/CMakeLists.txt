########################################################
set(target_binding comptonSoft)
set(swig_if_file interface/comptonSoft.i)
set(additional_swig_if_file interface/decayChains.i)
set(class_list_file interface/class_list_comptonSoft.hh)

########################################################
set(cxx_definitions )
set(swig_flags )
if(CS_USE_SIMX)
  list(APPEND cxx_definitions USE_SIMX)
  list(APPEND swig_flags -DUSE_SIMX)
endif()
if(CS_USE_FITSIO)
  list(APPEND cxx_definitions USE_FITSIO)
  list(APPEND swig_flags -DUSE_FITSIO)
endif()
if(CS_USE_HEALPIX)
  list(APPEND cxx_definitions USE_HEALPIX)
  list(APPEND swig_flags -DUSE_HEALPIX)
endif()
if(CS_USE_HSQUICKLOOK)
  list(APPEND cxx_definitions USE_HSQUICKLOOK)
  list(APPEND swig_flags -DUSE_HSQUICKLOOK)
endif()

########################################################
include(ANLNextSWIGRuby)

########################################################
anlnext_add_swig_ruby_binding(${target_binding}
  INTERFACE_FILE ${swig_if_file}
  INCLUDE_DIRECTORIES ${ANLG4_DIR}/bindings/ruby/interface interface
  LIBRARIES ComptonSoftCore ComptonSoftApplications
  SWIG_FLAGS ${swig_flags}
  COMPILE_DEFINITIONS ${cxx_definitions}
  INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby/comptonsoft
)

install(FILES ${SWIG_IF_FILE} ${ADDITIONAL_SWIG_IF_FILE} ${CLASS_LIST_FILE}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft/ruby)

########################################################
# install Ruby libraries written in Ruby
########################################################
install(DIRECTORY library/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby
  FILES_MATCHING PATTERN "*.rb")
