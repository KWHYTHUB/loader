cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(SapphireLoader VERSION 0.1.0 LANGUAGES C CXX)
set(PROJECT_VERSION_TYPE Alpha)

set(SAPPHIRE_NO_SAPPHIRE_FILE On)
set(SAPPHIRE_BUILD_CODEGEN On)
set(SAPPHIRE_NO_LINK_LOADER On)
set(SAPPHIRE_NO_IMPLICIT_ENTRY On)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(READ about.md LOADER_ABOUT_MD)
configure_file(src/internal/about.hpp.in ${CMAKE_SOURCE_DIR}/src/internal/about.hpp)

file(GLOB CORE_SOURCES
	src/cocos2d-ext/*.cpp
	src/core/*.cpp
	src/hooks/*.cpp
	src/internal/*.cpp
	src/internal/windows/*.cpp
	src/internal/mac/*.cpp
	src/internal/ios/*.cpp
	src/load/*.cpp
	src/load/windows/*.cpp
	src/mac/*.cpp
	src/main.cpp
	src/utils/*.cpp
	src/utils/windows/*.cpp
	src/utils/zip/*.cpp
)

# embed version info in binary
if (WIN32)
	configure_file(src/internal/windows/info.rc.in info.rc)
	set(CORE_SOURCES ${CORE_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/info.rc)
endif()

file(GLOB OBJC_SOURCES
	src/internal/ios/*.mm
	src/internal/mac/*.mm
	src/load/ios/*.mm
	src/load/mac/*.mm
	src/utils/ios/*.mm
	src/utils/mac/*.mm
)

# not recurse because dont include platform specific headers
file(GLOB HEADERS
	sdk/include/*.hpp
)

add_library(${PROJECT_NAME} SHARED
	${CORE_SOURCES}
	${OBJC_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
	src/internal/
	src/platform/
	src/gui/
	./ # lilac
)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/link)
target_link_libraries(${PROJECT_NAME} z lilac_hook) # lilac

if (WIN32)
	add_subdirectory(launcher/windows)
	target_include_directories(${PROJECT_NAME} PUBLIC dobby/include)
	target_link_directories(${PROJECT_NAME} PUBLIC dobby)
	target_link_libraries(${PROJECT_NAME} dobby)
endif()

add_definitions(-DSAPPHIRE_EXPORTING -DSAPPHIRE_EXPORTING_CODEGEN)

add_compile_definitions(SAPPHIRE_PLATFORM_CONSOLE)
find_path(SAPPHIRE_SDK_PATH
	NAMES Sapphire.cmake
	PATHS $ENV{SAPPHIRE_SUITE}/sdk /usr/local/sapphire/sdk ~/.sapphire/sdk
	DOC "Sapphire SDK path."
	REQUIRED
)

include(${SAPPHIRE_SDK_PATH}/Sapphire.cmake)

setup_sapphire_mod(BUILD_CODEGEN NO_LOADER NO_SAPPHIRE_FILE OUTPUT "Sapphire")

add_subdirectory(lilac)

if (APPLE) 
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR SAPPHIRE_TARGET_PLATFORM STREQUAL "iOS")
		add_custom_command(TARGET SapphireLoader
		    POST_BUILD COMMAND 
		    ${CMAKE_INSTALL_NAME_TOOL} -id \"/Library/MobileSubstrate/DynamicLibraries/Sapphire.dylib\"
		    $<TARGET_FILE:SapphireLoader>)
	endif()
endif()

set_source_files_properties(${OBJC_SOURCES} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_precompile_headers(${PROJECT_NAME} PUBLIC ${SAPPHIRE_SDK_PATH}/include/Sapphire.hpp)

if (WIN32)
	SET(DOBBY_GENERATE_SHARED OFF CACHE BOOL "Build dobby shared library" FORCE)
	# SET(DOBBY_DEBUG ON CACHE BOOL "Build dobby shared library" FORCE)
	add_subdirectory(dobby)

	target_link_libraries(${PROJECT_NAME} dbghelp)
endif()

if(NOT SAPPHIRE_DONT_BUILD_TEST_MODS)
	if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
		add_subdirectory(test)
	endif()
endif()
