#set(COMMON ../ui.h ../glu.c ../glu.h ../x11.c)

#find_package(GLUT REQUIRED)
#link_libraries(GL ${GLUT_LIBRARIES} X11 m)
add_executable(spinning-cube ${COMMON} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
include_directories(../..) # to include header <gles/api.h>
if(USE_UBUNTU_ENV_WORKAROUND)
    SET_TARGET_PROPERTIES(spinning-cube PROPERTIES LINKER_LANGUAGE CXX)
endif()

