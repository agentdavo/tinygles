cmake_minimum_required(VERSION 2.6)

project(tinygles)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(USE_UBUNTU_ENV_WORKAROUND TRUE)
if(USE_UBUNTU_ENV_WORKAROUND)
    include_directories(deps/libgles1-mesa-dev_12.0.6-0ubuntu0.16.04.1_amd64/include)
endif()

link_directories(${CMAKE_BINARY_DIR}/lib /usr/X11R6/lib)
#add_definitions(-g -std=gnu99 -O2 -ffast-math)
#add_definitions(-g -std=c++11 -O2 -ffast-math)
add_definitions(-g -std=c++11 -O0)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    add_definitions(-fsingle-precision-constant -mfpu=neon -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8)
endif()

#add_definitions(-DDEBUG_VERTEX_TRANSFORM)
#add_definitions(-DDEBUG_GLBEGIN_MATRIX)
#add_definitions(-DSHOW_WARNINGS_FOR_PROPER_IMPLEMENTATION)

include_directories(include)
add_subdirectory(src)
