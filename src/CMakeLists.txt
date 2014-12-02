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

# Source files
set(QUICKLY_SOURCES WorkerThread.cpp ThreadSafeMap.cpp ThreadPool.cpp DataAction.cpp)

# Create a shared and a static library
add_library(${QUICKLY_SHARED_LIBRARY_NAME} SHARED ${QUICKLY_SOURCES})
target_link_libraries(${QUICKLY_SHARED_LIBRARY_NAME} ${QUICKLY_REQUIRED_LIBRARIES})
add_library(${QUICKLY_STATIC_LIBRARY_NAME} ${QUICKLY_SOURCES})
target_link_libraries(${QUICKLY_STATIC_LIBRARY_NAME} ${QUICKLY_REQUIRED_LIBRARIES})

# Now we change the output name of "quickly-static.a" to "quickly.a"
# The library target "quickly" already has a default OUTPUT_NAME of "quickly", so we don't need to change it.
# The library target "quickly-static" has a default OUTPUT_NAME of "quickly-static", so change it.
set_target_properties(${QUICKLY_STATIC_LIBRARY_NAME} 
    PROPERTIES OUTPUT_NAME ${QUICKLY_SHARED_LIBRARY_NAME})
# Now the library target "quickly-static" will be named "quickly.lib" with MS tools.
# This conflicts with the "quickly.lib" import library corresponding to "quickly.dll",
# so we add a "lib" prefix (which is default on other platforms anyway):
set_target_properties(${QUICKLY_STATIC_LIBRARY_NAME} 
    PROPERTIES PREFIX "lib")

# Set the executable version
set_target_properties(${QUICKLY_SHARED_LIBRARY_NAME} ${QUICKLY_STATIC_LIBRARY_NAME}
    PROPERTIES VERSION ${QUICKLY_VERSION})

# Install the executables
install(TARGETS ${QUICKLY_SHARED_LIBRARY_NAME} 
    LIBRARY DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(TARGETS ${QUICKLY_STATIC_LIBRARY_NAME} 
    ARCHIVE DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
# This runs the 'ldconfig' command, because CMake doesn't
install(CODE "execute_process(COMMAND ldconfig)")
