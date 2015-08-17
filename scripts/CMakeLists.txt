if(USE_RUBY)
  install(DIRECTORY comptonsoft/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ruby/comptonsoft
    FILES_MATCHING PATTERN "*.rb")
endif(USE_RUBY)
