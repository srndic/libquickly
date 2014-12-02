# Copyright 2014 Nedim Srndic, University of Tuebingen
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

# Build and install libquickly test executable

# Source files
set(QUICKLY_TEST_EXECUTABLE_NAME test)
set(QUICKLY_TEST_EXECUTABLE_SOURCES main.cpp)

# Create the executable
add_executable(${QUICKLY_TEST_EXECUTABLE_NAME} ${QUICKLY_TEST_EXECUTABLE_SOURCES})
target_link_libraries(${QUICKLY_TEST_EXECUTABLE_NAME} ${QUICKLY_SHARED_LIBRARY_NAME})

# Set the executable version
set_target_properties(${QUICKLY_TEST_EXECUTABLE_NAME} PROPERTIES VERSION ${QUICKLY_VERSION})