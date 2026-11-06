include(ANLNextSwigRuby)

anlnext_add_swig_ruby_binding(${GEOMETRY_LIB}
  INTERFACE_FILE ${GEOMETRY_LIB}.i
  INCLUDE_DIRECTORIES
    ${ANLG4_INC_DIR}
    ${ANLNEXT_INC_DIR}
    ${G4_INC_DIR}
    ${ANLG4_INC_DIR}/ruby
    ${ANLNEXT_INC_DIR}/ruby
  LIBRARIES
    ${GEOMETRY_CXX_LIB}
    ${ANLG4_LIB}
    ${ANLNEXT_LIB}
    ${G4_LIB}
)
