cmake_minimum_required(VERSION 3.3)
project(RAPI)


IF( WIN32 )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	SET( PSDK_PATH "C:/Program Files/Microsoft Platform SDK/Bin" )
	SET( PSDK_INC "C:/Program Files/Microsoft Platform SDK/Include" )

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

	SET( DX11_LIBRARIES ${D3D11_LIBRARY} ${D3DX11_LIBRARY} )
	
	include_directories(${DX_INCLUDE_PATH})
ELSE()
	find_package(OpenGL REQUIRED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
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
ELSE()
include_directories(".")
include_directories("./renderer")
include_directories("./renderer/NULL")
include_directories("./renderer/base")
include_directories("./renderer/API")
include_directories("./renderer/utils")
include_directories("./renderer/D3D11")
include_directories("./renderer/GL")
ENDIF()

add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

IF( WIN32 )
add_library(RAPI WIN32 ${SOURCE_FILES})
ELSE()
add_library(RAPI ${SOURCE_FILES})
ENDIF()
target_link_libraries(RAPI ${OPENGL_LIBRARIES} ${DX11_LIBRARIES})

