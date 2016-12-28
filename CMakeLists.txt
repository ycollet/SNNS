cmake_minimum_required(VERSION 3.0.2)

project(SNNS)

option(SNNS_ENABLE_ENZO   "Enable the compilation of ENZO"   OFF)
option(SNNS_ENABLE_NESSUS "Enable the compilation of Nessus" OFF)
option(SNNS_ENABLE_VISTRA "Enable the compilation of Vistra" OFF)

# Compilation options
option(ENABLE_SHARED_LIBRARIES       "Build libraries as shared libraries" OFF)
option(COIN_COMPILE_STATIC           "Activate the static linking" OFF)
option(COIN_COMPILE_COVERAGE         "Activate the code coverage compilation" OFF)
option(COIN_COMPILE_PROFILE          "Activate the code profiling compilation" OFF)
option(COIN_COMPILE_PROFILE_VALGRIND "Activate the code profiling compilation for valgrind" OFF)
option(COIN_COMPILE_LTO              "Activate the whole program optimization (using LTO)" OFF)
option(COIN_COMPILE_WHOPR            "Activate the whole program optimization (using WHOPR)" OFF)
option(COIN_COMPILE_WARNINGS         "Activate a set of warning options" OFF)
option(COIN_COMPILE_CXX11            "Activate C++11 compilation" OFF)

mark_as_advanced(ENABLE_SHARED_LIBRARIES
                 COIN_COMPILE_STATIC
                 COIN_COMPILE_COVERAGE
                 COIN_COMPILE_PROFILE
                 COIN_COMPILE_PROFILE_VALGRIND
                 COIN_COMPILE_LTO
                 COIN_COMPILE_WHOPR
                 COIN_COMPILE_WARNINGS
                 COIN_COMPILE_CXX11)

# Set paths for binary and library generation inside the build directory:
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_SOURCE_DIR}/cmake)

Enable_Testing()

#------------------------------------------------------------
# Detect 64 bits
#------------------------------------------------------------

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(HAVE_64_BIT 0)
else ()
  set(HAVE_64_BIT 1)
endif ()

# Name of package
set(PACKAGE           "SNNS")
# Define to the address where bug reports for this package should be sent.
set(PACKAGE_BUGREPORT "bugs@snns.org")
# Define to the full name of this package.
set(PACKAGE_NAME      "SNNS")
# Define to the full name and version of this package.
set(PACKAGE_STRING    "SNNS")
# Define to the one symbol short name of this package.
set(PACKAGE_TARNAME   "SNNS")
# Define to the version of this package.
set(PACKAGE_VERSION   "4.3")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/snns_binary" 
      CACHE PATH "SNNS install prefix" FORCE)
endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#-----------------------------------------------------------------------------
# Manage compilation options
#-----------------------------------------------------------------------------

if (UNIX)
  if (COIN_COMPILE_STATIC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -z muldefs")
    endif ()
  elseif (COIN_COMPILE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage -fno-inline")
    endif ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -Xclang -coverage-cfg-checksum -Xclang -coverage-no-function-names-in-data -Xclang -coverage-version='408*'")
    endif ()
  elseif (COIN_COMPILE_PROFILE)
    set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
  elseif (COIN_COMPILE_PROFILE_VALGRIND)
    set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -p")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -p")
  else ()
    set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -Wno-deprecated -fPIC")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z muldefs")
    endif ()
  endif ()
  
  if (COIN_COMPILE_LTO)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
      set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -lto")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lto")
    endif ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -flto")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
    endif ()
  elseif (COIN_COMPILE_WHOPR)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -fwhopr")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fwhopr")
    endif ()
  endif ()
elseif (WIN32)
  if (MSVC)
    if (COIN_COMPILE_LTO OR COIN_COMPILE_WHOPR)
      set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} /GL ")
      set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} /GL ")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG ")
    endif ()
  endif ()
endif ()

# End detect fortran mangling

if (NOT WIN32 AND COIN_COMPILE_WARNINGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -fPIC ")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wno-deprecated -fPIC ")
  # Try to locate unitizalized variables
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wshadow -Wuninitialized ")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Wshadow -Wmissing-prototypes -Wuninitialized ")
endif ()

# Manage coverage via lcov automatically

if (COIN_COMPILE_COVERAGE)
  if (WIN32)
    message(FATAL_ERROR "Error: code coverage analysis is only available under Linux for now.")
  endif ()
  
  find_program(GCOV_PATH gcov)
  find_program(LCOV_PATH lcov)
  find_program(GENHTML_PATH genhtml)

  if (NOT GCOV_PATH)
    message(FATAL_ERROR "gcov not found! Please install lcov and gcov. Aborting...")
  endif ()
  
  if (NOT LCOV_PATH)
    message(FATAL_ERROR "lcov not found! Please install lcov and gcov. Aborting...")
  endif ()
  
  if (NOT GENHTML_PATH)
    message(FATAL_ERROR "genhtml not found! Please install lcov and gcov. Aborting...")
  endif ()
  
  # /!\ FAILURE IF AT LEAST ONE TEST FAILS
  # Capturing lcov counters and generating report
  add_custom_target(coverage
    COMMAND ${LCOV_PATH} --directory . --zerocounters
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} ${CMAKE_CTEST_COMMAND} -LE LONG -LE FAIL
    COMMAND ${LCOV_PATH} --capture --directory . --output-file ${CMAKE_BINARY_DIR}/coverage.info
    COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/coverage.info '/usr/*' '${CMAKE_BINARY_DIR}/Dependencies/*' --output-file ${CMAKE_BINARY_DIR}/coverage.info.cleaned
    COMMAND ${GENHTML_PATH} -o ${CMAKE_BINARY_DIR}/coverage ${CMAKE_BINARY_DIR}/coverage.info.cleaned
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/coverage.info ${CMAKE_BINARY_DIR}/coverage.info.cleaned
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Resetting code coverage counters to zero.
Processing code coverage counters and generating report.
You can zip the directory ${CMAKE_BINARY_DIR}/coverage and upload the content to a web server.")
endif ()

# End of coverage

include(CheckIncludeFile)

check_include_file("alloca.h"   HAVE_ALLOCA_H)
check_include_file("sys/wait.h" HAVE_SYS_WAIT_H)
check_include_file("dirent.h"   HAVE_DIRENT_H)
check_include_file("fcntl.h"    HAVE_FCNTL_H)
check_include_file("limits.h"   HAVE_LIMITS_H)
check_include_file("ndir.h"     HAVE_NDIR_H)
check_include_file("sys/dir.h"  HAVE_SYS_DIR_H)
check_include_file("sys/ndir.h" HAVE_SYS_NDIR_H)
check_include_file("sys/time.h" HAVE_SYS_TIME_H)
check_include_file("unistd.h"   HAVE_UNISTD_H)
check_include_file("values.h"   HAVE_VALUES_H)

include(CheckCXXCompilerFlag)

if (COIN_COMPILE_CXX11 AND NOT WIN32)
  # VS2013 activate C++11 by default
  check_cxx_compiler_flag(-std=c++11 HAVE_STD_CPP11_FLAG)
  if (HAVE_STD_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else ()
    check_cxx_compiler_flag(-std=c++0x HAVE_STD_CPP0X_FLAG)
    if (HAVE_STD_CPP0X_FLAG)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif ()
  endif ()
endif ()

check_cxx_compiler_flag(-Qunused-arguments HAVE_QUNUSED_ARGUMENTS)
if (HAVE_QUNUSED_ARGUMENTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
endif ()

include(CheckTypeSize)

CHECK_TYPE_SIZE("size_t" SIZEOF_SIZE_T)
if (NOT SIZEOF_SIZE_T)
  set(size_t "unsigned int")
endif ()

include(TestSignalType)
include(AC_HEADER_STDC)
include(CheckFunctionExists)

set(CMAKE_REQUIRED_LIBRARIES m)
check_function_exists(getcwd HAVE_GETCWD)
check_function_exists(lrand48 HAVE_LRAND48)
check_function_exists(strdup HAVE_LRAND48)
check_function_exists(alloca HAVE_ALLOCA)

find_program(HAVE_XGRABSC xgrabsc)

configure_file(${CMAKE_SOURCE_DIR}/cmake/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

#-----------------------------------------------
# Doxygen documentation
#-----------------------------------------------

#set(doc_dir doc)
#find_package(Doxygen)
#
#if(DOXYGEN AND ENABLE_DOXYGEN)
#  configure_file(${CMAKE_SOURCE_DIR}/Cgl/doxydoc/doxygen.conf.in       ${CMAKE_SOURCE_DIR}/Cgl/doxydoc/doxygen.conf)
#
#  add_custom_target(docs 
#                    ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Cgl/doxydoc/doxygen.conf)
#endif()

subdirs(kernel)
subdirs(tools)
subdirs(xgui)
if (SNNS_ENABLE_ENZO)
  subdirs(contrib/enzo)
endif ()
if (SNNS_ENABLE_NESSUS)
  subdirs(contrib/nessus)
endif ()
if (SNNS_ENABLE_VISTRA)
  subdirs(contrib/vistra)
endif ()

#
# Packaging
#

set(CPACK_PACKAGE_NAME      "${PACKAGE_NAME}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}")

message(STATUS "Package filename: ${CPACK_PACKAGE_FILE_NAME}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PACKAGE_NAME}")
set(CPACK_PACKAGE_VERSION "${PACKAGE_VERSION}")

set(CPACK_PACKAGE_RELOCATABLE TRUE)

if (WIN32)
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}")
  set(CPACK_MONOLITHIC_INSTALL ON)
  set(CPACK_NSIS_CONTACT "${PACKAGE_BUGREPORT}")
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_NSIS_PACKAGE_NAME "SNNS ${PACKAGE_VERSION}")
elseif (APPLE)
  set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
  set(CPACK_PACKAGE_DEFAULT_LOCATION "/opt/${CPACK_PACKAGE_NAME}")
  set(CPACK_PACKAGING_INSTALL_PREFIX "/")
else (WIN32)
  set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
  set(CPACK_PACKAGE_DEFAULT_LOCATION "/opt/${CPACK_PACKAGE_NAME}")
  set(CPACK_PACKAGING_INSTALL_PREFIX "/")
  
  set(CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_BUGREPORT}")
  
  set(CPACK_RPM_COMPONENT_INSTALL ON)
  set(CPACK_RPM_PACKAGE_RELOCATABLE ON)
  set(CPACK_RPM_PACKAGE_LICENSE "Copyrighted")
  set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
  set(CPACK_RPM_PACKAGE_URL "${PACKAGE_URL}")
  set(CPACK_RPM_PACKAGE_SUMMARY "Cbc is MIP solver library from CoinOR.")
  set(CPACK_RPM_PACKAGE_DESCRIPTION 
    "The Cbc suite includes:

    * A set of linear solvers to manager the resolution of various problems.
"
    )
endif ()

include(CPack)

###########################
#                         #
# Sum-up of configuration #
#                         #
###########################

message(STATUS "************************")
message(STATUS "*                      *")
message(STATUS "* Configuration sum-up *")
message(STATUS "*                      *")
message(STATUS "************************")
message(STATUS "")

message(STATUS "Installation directory: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

get_directory_property(ALL_INCLUDES INCLUDE_DIRECTORIES)
get_directory_property(ALL_LINKS    LINK_DIRECTORIES)
get_directory_property(ALL_DEFS     COMPILE_DEFINITIONS)

message(STATUS "Include directories:     ${ALL_INCLUDES}")
message(STATUS "Link directories:        ${ALL_LINKS}")
message(STATUS "Compilation definitions: ${ALL_DEFS}")
