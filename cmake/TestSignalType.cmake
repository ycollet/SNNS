# cmake/modules/TestSignalType.cmake
#
# Test for the return type of signal defined in <signal.h>
#
# Copyright (C) 2006  Andrew Ross
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# If signal.h defines signal as returning a pointer to a function 
# returning void RETSIGTYPE is defined as void. Otherwise RETSIGTYPE is
# defined as int.

if (NOT DEFINED CMAKE_TEST_SIGNAL_TYPE)
  message(STATUS "Check for signal return type in <signal.h>")
  try_compile(CMAKE_TEST_SIGNAL_TYPE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/TestSignalType.c
    OUTPUT_VARIABLE OUTPUT)
  if (CMAKE_TEST_SIGNAL_TYPE)
    message(STATUS "Check for signal handler return type type void  - found")
    set(RETSIGTYPE void CACHE INTERNAL "Signal return type")
    file(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if signal handler return type is void passed with "
      "the following output:\n${OUTPUT}\n\n")
  else ()
    message(STATUS "Check for signal handler return type type void  - not found")
    set(RETSIGTYPE int CACHE INTERNAL "Signal return type")
    file(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if signal handler return type is void failed with "
      "the following output:\n${OUTPUT}\n\n")
  endif ()
endif ()
