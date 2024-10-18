cmake_minimum_required(VERSION 3.12)
project(tinygles)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directories
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(LIB_DIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})

# Add X11 library search path
link_directories(/usr/X11R6/lib)

# Define source files for the GLES project
set(GLES_SRC
    init.cpp
    api.cpp
    arrays.cpp
    blend.cpp
    clear.cpp
    clip.cpp
    enable.cpp
    error.cpp
    fog.cpp
    get.cpp
    gl_str.c
    glx.cpp
    image_util.cpp
    light.cpp
    matrix.cpp
    misc.cpp
    specbuf.cpp
    stub.cpp
    tack.c
    texture.cpp
    vertex.cpp
    zbuffer.cpp
    zdither.cpp
    zline.cpp
    ztriangle.cpp
    zmath.cpp
    context.cpp
    fixed_point_operations.cpp
    math_tests.cpp
    matrix_utils.cpp
    x11.cpp
    main.cpp
)

# Define header files for the GLES project
set(GLES_HDR
    mygl.h
    api.hpp
    arrays.hpp
    image_util.hpp
    fixed_point.hpp
    fixed_point_type.hpp
    fixed_point_operations.hpp
    math_tests.hpp
    gl.h
    get.hpp
    glx.hpp
    gl_str.h
    light.hpp
    matrix.hpp
    misc.hpp
    specbuf.hpp
    tack.h
    vertex.hpp
    zbuffer.hpp
    zgl.hpp
    zmath.hpp
    clear.hpp
    matrix_utils.hpp
    x11.hpp
)

# Add executable for the project
add_executable(tinygles ${GLES_SRC})

# Include directories for header files
target_include_directories(tinygles PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link the X11 libraries
find_library(X11_LIB X11 REQUIRED)
find_library(XEXT_LIB Xext REQUIRED)
target_link_libraries(tinygles PRIVATE ${X11_LIB} ${XEXT_LIB})

# Find and link OpenGL ES library
find_library(GLES_LIB GLESv1_CM REQUIRED)
if (GLES_LIB)
    target_link_libraries(tinygles PUBLIC ${GLES_LIB})
else()
    message(FATAL_ERROR "Could not find GLESv1_CM library.")
endif()

# Link math library (-lm) if found
find_library(M_LIB m)
if (M_LIB)
    target_link_libraries(tinygles PRIVATE ${M_LIB})
else()
    message(WARNING "Could not find math library (-lm).")
endif()

# Optionally, link to pthreads if needed
find_package(Threads REQUIRED)
target_link_libraries(tinygles PRIVATE Threads::Threads)
