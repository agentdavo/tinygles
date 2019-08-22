#set(COMMON ../ui.h ../glu.c ../glu.h ../x11.c)
set(COMMON ../ui.h ../x11.cpp ../x11.hpp)

add_executable(gears ${COMMON} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
include_directories(../..) # to include header <gles/api.h>
if(USE_UBUNTU_ENV_WORKAROUND)
    SET_TARGET_PROPERTIES(gears PROPERTIES LINKER_LANGUAGE CXX)
endif()
#target_link_libraries(gears GLESv1_CM X11 Xext)
target_link_libraries(gears GLESv1_CM X11 Xext -lGL)

