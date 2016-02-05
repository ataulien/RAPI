cmake_minimum_required(VERSION 2.4)
project(GLEW)

find_package (OpenGL REQUIRED)
set (GLEW_LIBRARIES ${OPENGL_LIBRARIES})

if ( NOT DEFINED CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type" )
endif ()

if (COMMAND cmake_policy)
  cmake_policy (SET CMP0003 NEW)
endif()

if (WIN32)
  set (GLEW_LIB_NAME glew32)
else ()
  set (GLEW_LIB_NAME GLEW)
  set (DLL_PREFIX lib)
endif ()

set (GLEW_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
        "include"
)


add_library (glew SHARED ${GLEW_DIR}/src/glew.c)
set_target_properties (glew PROPERTIES COMPILE_DEFINITIONS "GLEW_BUILD" OUTPUT_NAME "${GLEW_LIB_NAME}" PREFIX "${DLL_PREFIX}")
add_library (glew_s STATIC ${GLEW_DIR}/src/glew.c)
set_target_properties (glew_s PROPERTIES COMPILE_DEFINITIONS "GLEW_STATIC" OUTPUT_NAME "${GLEW_LIB_NAME}" PREFIX lib)
target_link_libraries (glew ${GLEW_LIBRARIES})
target_link_libraries (glew_s ${GLEW_LIBRARIES})