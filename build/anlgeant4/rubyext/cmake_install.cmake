# Install script for directory: /Users/takashima/software/ComptonSoft/anlgeant4/rubyext

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/takashima/software/ComptonSoft/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/takashima/software/ComptonSoft/install/lib/ruby/anlgeant4/anlGeant4.bundle")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/takashima/software/ComptonSoft/install/lib/ruby/anlgeant4" TYPE MODULE FILES "/Users/takashima/software/ComptonSoft/build/anlgeant4/rubyext/anlGeant4.bundle")
  if(EXISTS "$ENV{DESTDIR}/Users/takashima/software/ComptonSoft/install/lib/ruby/anlgeant4/anlGeant4.bundle" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/takashima/software/ComptonSoft/install/lib/ruby/anlgeant4/anlGeant4.bundle")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/takashima/software/ComptonSoft/build/anlgeant4"
      -add_rpath "/Users/takashima/software/ComptonSoft/install/lib"
      -add_rpath "/opt/homebrew/Cellar/root/6.38.04_1/lib/root"
      "$ENV{DESTDIR}/Users/takashima/software/ComptonSoft/install/lib/ruby/anlgeant4/anlGeant4.bundle")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" -x "$ENV{DESTDIR}/Users/takashima/software/ComptonSoft/install/lib/ruby/anlgeant4/anlGeant4.bundle")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/takashima/software/ComptonSoft/install/include/anlgeant4/ruby/anlGeant4.i;/Users/takashima/software/ComptonSoft/install/include/anlgeant4/ruby/class_list_anlGeant4.hh")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/takashima/software/ComptonSoft/install/include/anlgeant4/ruby" TYPE FILE FILES
    "/Users/takashima/software/ComptonSoft/anlgeant4/rubyext/interface/anlGeant4.i"
    "/Users/takashima/software/ComptonSoft/anlgeant4/rubyext/interface/class_list_anlGeant4.hh"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/takashima/software/ComptonSoft/install/lib/ruby/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Users/takashima/software/ComptonSoft/install/lib/ruby" TYPE DIRECTORY FILES "/Users/takashima/software/ComptonSoft/anlgeant4/rubyext/library/" FILES_MATCHING REGEX "/[^/]*\\.rb$")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/takashima/software/ComptonSoft/build/anlgeant4/rubyext/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
