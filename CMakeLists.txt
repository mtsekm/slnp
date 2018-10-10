cmake_minimum_required(VERSION 3.9)
project(slnp VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_DOC "Build documentation" ON)
option(ENABLE_LOGGER "Enable logger" ON)

string(LENGTH "${CMAKE_SOURCE_DIR}/src/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)

IF(ENABLE_LOGGER)
	ADD_DEFINITIONS(-DSLNP_ENABLE_LOGGER)
ENDIF(ENABLE_LOGGER)

## doxygen API docs
if (BUILD_DOC)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxy.in)
		set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
		configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	    add_custom_target( doc ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM )
	else (DOXYGEN_FOUND)
		message("Doxygen need to be installed to generate the doxygen documentation")
	endif (DOXYGEN_FOUND)
endif(BUILD_DOC)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_subdirectory(src)
add_subdirectory(example)