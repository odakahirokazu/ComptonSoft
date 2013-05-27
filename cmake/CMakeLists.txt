if(INSTALL_CMAKE_FILES)
  install(FILES UseComptonSoft.cmake external_libraries.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/comptonsoft)
endif(INSTALL_CMAKE_FILES)
