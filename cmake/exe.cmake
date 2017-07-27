add_definitions(-DANLNEXT_USE_TVECTOR -DANLNEXT_USE_HEPVECTOR)

include_directories(include
  ${COMPTONSOFT}/source/tools/include
  ${COMPTONSOFT}/source/modules/include
  ${COMPTONSOFT}/source/simulation/include
  ${ANLGEANT4_DIR}/include
  ${ANLNEXT_DIR}/include
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${CLHEP_INC_DIR}
  ${ADD_INC_DIR}
  )

link_directories(
  ${COMPTONSOFT_BUILD}/lib
  ${AN_LIB_INSTALL_PATH}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${ADD_LIB_DIR}
  )

set(ANLNEXT_DEF_DIR src)
set(ANLNEXT_BIN_DIR ${COMPTONSOFT_BUILD}/bin)

set(ANLNEXT_EXE_LIB
  CSSimulation CSModules CSTools
  ANLGeant4 ${ROOT_LIB} ${G4_LIB} ${CLHEP_LIB} ${ADD_LIB}
  )

set(USEROOT 1)

include(gen_anl)
