include(external_libraries)

if(NOT DEFINED CS_INSTALL)
  if($ENV{CS_INSTALL})
    set(CS_INSTALL $ENV{CS_INSTALL})
  else($ENV{CS_INSTALL})
    set(CS_INSTALL $ENV{HOME})
  endif($ENV{CS_INSTALL})
endif(NOT DEFINED CS_INSTALL)
set(CS_INC_DIR ${CS_INSTALL}/include/comptonsoft)
set(CS_LIB_DIR ${CS_INSTALL}/lib)
set(CS_LIB CSTools CSModules CSSimulation)
message("-- CS_INSTALL = ${CS_INSTALL}")

### ANLGeant4
if(NOT DEFINED ANLG4_INSTALL)
  set(ANLG4_INSTALL ${CS_INSTALL})
endif(NOT DEFINED ANLG4_INSTALL)
set(ANLG4_INC_DIR ${ANLG4_INSTALL}/include/anlgeant4)
set(ANLG4_LIB_DIR ${ANLG4_INSTALL}/lib)
set(ANLG4_LIB ANLGeant4)
message("-- ANLG4_INSTALL = ${ANLG4_INSTALL}")

### Additional ###
set(ADD_INC_DIR )
set(ADD_LIB_DIR )
set(ADD_LIB )

if(UNIX)
  if(NOT APPLE)
    set(ADD_LIB ${ADD_LIB} boost_system)
  endif(NOT APPLE)
endif(UNIX)
