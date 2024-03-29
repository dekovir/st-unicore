cmake_minimum_required(VERSION 3.8.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

# PROJECT ######################################################################
project(UniCore)

set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LIBS_DIR "${PROJECT_DIR}/libs/")
set(PLUGINS_DIR "${PROJECT_DIR}/plugins/")

function(unicore_group_sources_by_folder target)
	set(last_dir "")
	set(files "")

	get_target_property(sources ${target} SOURCES)

	foreach(file ${sources})
		file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_LIST_DIR}" ${file})
		get_filename_component(dir "${relative_file}" DIRECTORY)
		if(NOT "${dir}" STREQUAL "${last_dir}")
			if(files)
				source_group("${last_dir}" FILES ${files})
			endif()
			set(files "")
		endif()
		set(files ${files} ${file})
		set(last_dir "${dir}")
	endforeach()

	if(files)
		source_group("${last_dir}" FILES ${files})
	endif()
endfunction()

function(unicore_init_target target)
	if(MSVC)
		target_compile_options(${target} PRIVATE "/MP")
	#	target_compile_options(${target} PRIVATE /W4 /WX)
	#else()
	#	target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	endif()
endfunction()

# UNICORE ######################################################################
set(UNICORE_DIR "${PROJECT_DIR}/src")
set(UNICORE_INCLUDE "${PROJECT_DIR}/include")

file(GLOB_RECURSE UNICORE_SOURCE "${UNICORE_DIR}/*.cpp" "${UNICORE_DIR}/*.hpp" "${UNICORE_INCLUDE}/*.hpp")

add_library(unicore STATIC ${UNICORE_SOURCE})
unicore_init_target(unicore)
unicore_group_sources_by_folder(unicore)
target_include_directories(unicore PUBLIC "${UNICORE_INCLUDE}")
target_include_directories(unicore PRIVATE "${UNICORE_DIR}")
set_target_properties(unicore PROPERTIES FOLDER "UniCore")

# SDL2
if (NOT EMSCRIPTEN)
	set(SDL_STATIC ON CACHE BOOL "" FORCE)
	set(SDL_SHARED OFF CACHE BOOL "" FORCE)
	set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
	set(SDL_LIBC ON CACHE BOOL "" FORCE)
	set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
	set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)
	set(SDL_TEST_ENABLED_BY_DEFAULT OFF CACHE BOOL "" FORCE)

	add_subdirectory("${LIBS_DIR}/SDL2")
	set_target_properties(SDL2-static SDL2main PROPERTIES FOLDER "Libs")
	target_link_libraries(unicore PRIVATE SDL2-static)
else()
	target_link_options(unicore PUBLIC -sUSE_SDL=2)
	target_compile_options(unicore PUBLIC -sUSE_SDL=2)
endif()
target_compile_definitions(unicore PUBLIC UNICORE_USE_SDL2)

if (WIN32)
	target_link_libraries(unicore PRIVATE Shlwapi)
endif()

#target_compile_options(unicore PUBLIC -fno-exceptions)

# PLUGINS ######################################################################
add_subdirectory(${PLUGINS_DIR})

# MAIN #########################################################################
set(MAIN_DIR "${PROJECT_DIR}/main")
file(GLOB MAIN_SRC "${MAIN_DIR}/*.cpp" "${MAIN_DIR}/*.hpp")

# FUNCTIONS ####################################################################
function(unicore_init_executable target_name)
	if (WIN32)
		set_target_properties(${target_name}
			PROPERTIES
				WIN32_EXECUTABLE ON
		)
	elseif(EMSCRIPTEN)
		target_link_libraries(${target_name} PRIVATE
			"--bind"
			"--emrun"
			"-s ALLOW_MEMORY_GROWTH=1"
		)
		set_target_properties(${target_name}
			PROPERTIES
				SUFFIX ".html"
		)
	elseif(UNIX OR ANDROID)
	else()
		message(FATAL_ERROR "Unsupported platform")
	endif()

	unicore_init_target(${target_name})
	target_link_libraries(${target_name} PRIVATE unicore)
	target_sources(${target_name} PUBLIC ${MAIN_SRC})
	target_include_directories(${target_name} PUBLIC "${MAIN_DIR}")

	if (NOT EMSCRIPTEN)
		target_link_libraries(${target_name} PRIVATE SDL2main)
		target_compile_definitions(${target_name} PUBLIC UNICORE_USE_SDL2_MAIN)
	endif()

	if (MSVC)
		set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${target_name})
	endif()
endfunction()

function(unicore_init_assets target_name directory)
	if(EMSCRIPTEN)
		target_link_libraries(${target_name} PRIVATE
			"--preload-file ${directory}@/"
		)
	endif()
	if (MSVC)
		set_target_properties(${target_name} PROPERTIES
			VS_DEBUGGER_WORKING_DIRECTORY "${directory}"
		)
	endif()
endfunction()


# EXAMPLES #####################################################################
set(EXAMPLES_DIR "${PROJECT_DIR}/examples")

add_subdirectory(${EXAMPLES_DIR})