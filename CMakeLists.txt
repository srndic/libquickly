# Copyright 2012 Nedim Srndic, University of Tuebingen
# 
# This file is part of libquickly.
#
# libquickly is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# libquickly is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with libquickly.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8)
project(libquickly)
set(QUICKLY_VERSION "1.1")

# A macro for aborting if a required library is not found
macro(require_library RL_LIBRARY_NAME)
	find_library(RL_LIB ${RL_LIBRARY_NAME})
	if (RL_LIB STREQUAL "RL_LIB-NOTFOUND")
		message(FATAL_ERROR "The library '${RL_LIBRARY_NAME}' is required, not found")
	else (RL_LIB STREQUAL "RL_LIB-NOTFOUND")
		message(STATUS "Found library '${RL_LIBRARY_NAME}'")
	endif (RL_LIB STREQUAL "RL_LIB-NOTFOUND")
	unset(RL_LIB CACHE)
	unset(RL_LIB)
endmacro(require_library)

# If boost_thread is not installed, do not proceed
require_library(boost_thread-mt)

# Set some important names
set(QUICKLY_SHARED_LIBRARY_NAME quickly)
set(QUICKLY_STATIC_LIBRARY_NAME quickly-static)
set(QUICKLY_REQUIRED_LIBRARIES boost_thread-mt)

# Set default compile flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wformat=2 -Wall -Wextra -Wl,-z,defs")
endif(CMAKE_COMPILER_IS_GNUCXX)

# Source directory
add_subdirectory(src)

# Test directory
if (MAKE_TESTS)
    message(STATUS "Will make tests")
    add_subdirectory(test)
endif (MAKE_TESTS)
unset(MAKE_TESTS CACHE)
unset(MAKE_TESTS)

# Install include files
install(DIRECTORY src/
        DESTINATION include/quickly
        FILES_MATCHING PATTERN "*.h"
                       PATTERN ".svn" EXCLUDE)
