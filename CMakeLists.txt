cmake_minimum_required(VERSION 3.3)
project(RAPI)

find_package(OpenGL REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

option(RAPI_D3D11 "Enable RAPI-D3D11-Build" ON)

if( WIN32 )
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /DEBUG")
	
	if(RAPI_D3D11)
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
	else()
		SET(DX_INCLUDE_PATH "")
		SET(D3D11_LIBRARY "")
		SET(D3DX11_LIBRARY "")

	endif()
endif() 

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

if( WIN32 )
	include_directories("/")
	include_directories("renderer")
	include_directories("renderer/NULL")
	include_directories("renderer/base")
	include_directories("renderer/API")
	include_directories("renderer/utils")
	include_directories("renderer/D3D11")
	include_directories("renderer/GL")
	include_directories("renderer/include")

	add_subdirectory(lib/glew)

	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		set(GLFW_LIB_DIR "lib/glfw/x64")
	else()
		set(GLFW_LIB_DIR "lib/glfw/Win32")
	endif()

	link_directories(${GLFW_LIB_DIR})
	link_directories(lib/glew/lib)

	set( GLEW_LIBRARIES glew glu32 glfw)

else()
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
endif()

include_directories("include")

add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")



if( WIN32 )
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
endif()



add_library(RAPI_GL STATIC ${SOURCE_FILES})

set_target_properties(RAPI_GL PROPERTIES
    COMPILE_PDB_NAME "RAPI_GL"
    COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

target_link_libraries(RAPI_GL ${OPENGL_LIBRARIES} ${DX11_LIBRARIES} ${GLEW_LIBRARIES})
target_compile_options(RAPI_GL PUBLIC -DRND_GL)

if( WIN32 )
	if(RAPI_D3D11)
		add_library(RAPI_DX STATIC ${SOURCE_FILES})

		set_target_properties(RAPI_DX PROPERTIES
			COMPILE_PDB_NAME "RAPI_DX"
			COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

		target_link_libraries(RAPI_DX ${OPENGL_LIBRARIES} ${DX11_LIBRARIES} ${GLEW_LIBRARIES})
		target_compile_options(RAPI_DX PUBLIC -DRND_D3D11)
	endif()
endif()
	
	
	
	
	
	
	
	
	
	
	
	
	