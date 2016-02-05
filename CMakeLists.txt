cmake_minimum_required(VERSION 3.3)
project(RAPI)

find_package(OpenGL REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

IF( WIN32 )
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /DEBUG")
	SET( PSDK_PATH "C:/Program Files/Microsoft Platform SDK/Bin" )
	SET( PSDK_INC "C:/Program Files/Microsoft Platform SDK/Include" )

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	FIND_PATH(DX_INCLUDE_PATH d3d11.h
		PATHS
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where D3D11.h resides")


	FIND_LIBRARY(D3D11_LIBRARY d3d11.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x64"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x64"
		DOC "The directory where d3d11.lib resides")

	FIND_LIBRARY(D3DX11_LIBRARY d3dx11.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x64"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x64"
		DOC "The directory where d3dx11.lib resides")
else()
	FIND_PATH(DX_INCLUDE_PATH d3d11.h
		PATHS
			"$ENV{DXSDK_DIR}/Include"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Include"
		DOC "The directory where D3D11.h resides")


	FIND_LIBRARY(D3D11_LIBRARY d3d11.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
		DOC "The directory where d3d11.lib resides")

	FIND_LIBRARY(D3DX11_LIBRARY d3dx11.lib
		PATHS
			"$ENV{DXSDK_DIR}/Lib/x86"
			"$ENV{PROGRAMFILES}/Microsoft DirectX SDK/Lib/x86"
		DOC "The directory where d3dx11.lib resides")
endif()
		
	SET( DX11_LIBRARIES ${D3D11_LIBRARY} ${D3DX11_LIBRARY} )
	
	include_directories(${DX_INCLUDE_PATH})
ELSE()
	
ENDIF() 

file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "renderer/*.h"
        "renderer/*.cpp"
        "renderer/D3D11/*.h"
        "renderer/D3D11/*.cpp"
        "renderer/NULL/*.h"
        "renderer/NULL/*.cpp"
        "renderer/API/*.h"
        "renderer/API/*.cpp"
        "renderer/base/*.h"
        "renderer/base/*.cpp"
        "renderer/utils/*.h"
        "renderer/utils/*.cpp"
		"renderer/GL/*.h"
        "renderer/GL/*.cpp"
		"renderer/include/*.h"
        "renderer/include/*.cpp"
)

if (MSVC)
    foreach( src_file ${SOURCE_FILES} )
        set_source_files_properties(
            ${src_file}
            PROPERTIES
            COMPILE_FLAGS "/Yupch.h"
            )
			
	set_source_files_properties(pch.cpp
        PROPERTIES
        COMPILE_FLAGS "/Ycpch.h"
        )
    endforeach( src_file ${SOURCE_FILES} )
endif (MSVC)

IF( WIN32 )
include_directories("/")
include_directories("renderer")
include_directories("renderer/NULL")
include_directories("renderer/base")
include_directories("renderer/API")
include_directories("renderer/utils")
include_directories("renderer/D3D11")
include_directories("renderer/GL")
include_directories("renderer/include")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
set(GLFW_LIB_DIR "lib/glfw/x64")
else()
set(GLFW_LIB_DIR "lib/glfw/Win32")
endif()

link_directories(${GLFW_LIB_DIR})
link_directories(lib/glew/lib)

SET( GLEW_LIBRARIES glew glu32 glfw3)


ELSE()
include_directories(".")
include_directories("./renderer")
include_directories("./renderer/NULL")
include_directories("./renderer/base")
include_directories("./renderer/API")
include_directories("./renderer/utils")
include_directories("./renderer/D3D11")
include_directories("./renderer/GL")
include_directories("./renderer/include")
#SET( GLEW_LIBRARIES "GLEW" "GLU" "glfw3")
ENDIF()

include_directories("include")

add_subdirectory(lib/glew)

add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")



IF( WIN32 )
add_executable(RAPI_EXE WIN32 ${SOURCE_FILES})
target_link_libraries(RAPI_EXE ${OPENGL_LIBRARIES} ${DX11_LIBRARIES} ${GLEW_LIBRARIES})
else()
    find_package(GLEW)
    if(!GLEW_FOUND)
        message(FATAL_ERROR "Glew not found")
    endif()
    include_directories(${GLEW_INCLUDE_DIRS})
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    message( ${GLEW_INCLUDE_DIRS})
    include_directories(${GLFW_INCLUDE_DIRS})
    add_executable(rapi_test ${SOURCE_FILES})
    target_link_libraries(rapi_test ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_STATIC_LIBRARIES})
ENDIF()



add_library(RAPI STATIC ${SOURCE_FILES})

set_target_properties(RAPI PROPERTIES
    COMPILE_PDB_NAME "RAPI"
    COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

target_link_libraries(RAPI ${OPENGL_LIBRARIES} ${DX11_LIBRARIES} ${GLEW_LIBRARIES})

if(MSVC)
	#FILE(WRITE dummy.c "void main(){}")
    #
	#set(EXT_LIB ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_STATIC_LIBRARIES}) 
	#set(BIG_LIB test)
	#add_library (project2 dummy.c)
	#get_property(PROJ2_LOC TARGET project2 PROPERTY LOCATION_<CONFIG>)

	#add_custom_command(OUTPUT ${BIG_LIB} 
	#				   DEPENDS project2
	#				   COMMAND "lib.exe /OUT:${BIG_LIB} ${EXT_LIB} $<TARGET_FILE:project2>" )
	
	#add_custom_target(tgt DEPENDS RAPI_SMALL COMMAND "${BIG_LIB} ${EXT_LIB} $<TARGET_FILE:project2>")
	#add_custom_target(tgt DEPENDS RAPI_SMALL COMMAND lib.exe /OUT:$<TARGET_PATH:RAPI_SMALL> ${CMAKE_SOURCE_DIR}/${GLFW_LIB_DIR}/glfw3.lib ${CMAKE_SOURCE_DIR}/${GLEW_LIB_DIR}/glew32.lib $<TARGET_FILE:project2> $<TARGET_FILE:RAPI_SMALL>)

endif()


	
	
	
	
	
	
	
	
	
	
	
	
	
	