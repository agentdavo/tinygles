set(COMMON ../x11.cpp ../x11.hpp)

add_executable(primitives ${COMMON} ${CMAKE_CURRENT_SOURCE_DIR}/primitives.cpp)
include_directories(../..) # to include header <gles/api.h>
target_link_libraries(primitives GLESv1_CM X11 Xext -lGL)
