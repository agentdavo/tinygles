set(COMMON ui.c x11.c x11.cpp)

add_executable(dummy ${COMMON} main.cpp)
include_directories(../..) # to include header <gles/api.h>
if(FORCE_CXX)
    SET_TARGET_PROPERTIES(dummy PROPERTIES LINKER_LANGUAGE CXX)
endif()
target_link_libraries(dummy GLESv1_CM X11 Xext -lGL)
