CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#INCLUDE(cmake/modules/comp_base.cmake)
#SET(CMAKE_VERBOSE_MAKEFILE on )
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# searches for needed packages
FIND_PACKAGE(ROOT REQUIRED)
FIND_PACKAGE(CLHEP REQUIRED)
FIND_PACKAGE(GSL REQUIRED)

#FIND_PACKAGE(GENFIT)

#FIND_PACKAGE(FTGL REQUIRED)
#FIND_PACKAGE(Geant4 REQUIRED COMPONENTS qt ui_tcsh)

# searches for needed packages
set(Boost_ADDITIONAL_VERSIONS "1.47" "1.47.0" "1.60" "1.60.0")
set(BOOST_INCLUDEDIR "$ENV{HOME}/Boost/boost_1_60_0/")

set (BUILD_WITHMPI off CACHE bool "Enable MPI for parallel processing (normally off)")

if (BUILD_WITHMPI)
 find_package(MPI REQUIRED)
 FIND_PACKAGE(Boost 1.47 COMPONENTS system program_options program_options mpi serialization REQUIRED) 
ELSE()
 FIND_PACKAGE(Boost 1.47 COMPONENTS system program_options program_options REQUIRED) 
ENDIF()

MESSAGE(STATUS "${Boost_LIBRARIES}" )

#FIND_PACKAGE( OpenMP)


# Set path to custom cmake modules for this project
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Flags to control installation progress

# Set some compiler flags
IF(NOT DEFINED RERUN)
	SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
ENDIF(NOT DEFINED RERUN)


	
SET(BINDIR "bin" CACHE STRING "binary directory within CMAKE_INSTALL_PREFIX")
SET(LIBDIR "lib" CACHE STRING "library directory within CMAKE_INSTALL_PREFIX")
SET(INCDIR "include" CACHE STRING "include directory within CMAKE_INSTALL_PREFIX")
SET(DATADIR "" CACHE STRING "data directory within CMAKE_INSTALL_PREFIX")

MESSAGE(STATUS "Your plugin will be installed in ${CMAKE_INSTALL_PREFIX}/${LIBDIR}")


# Set output path for libraries and executables within build directory (makes linking easier...)
SET(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# set prefix etc...
IF(NOT DEFINED RERUN)
	IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	  EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE ARCHDIR)
          FILE(MAKE_DIRECTORY "$ENV{HOME}/.muse/")
          FILE(MAKE_DIRECTORY "$ENV{HOME}/.muse/${ARCHDIR}/")
          FILE(MAKE_DIRECTORY "$ENV{HOME}/.muse/shared/")

          SET(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.muse/${ARCHDIR}/"
	  CACHE STRING "Prefix for tree installation" FORCE)


          SET(CMAKE_SHARED_PREFIX "$ENV{HOME}/.muse/shared"

	  CACHE STRING "Prefix for shared files installation" FORCE)

	  MESSAGE("Will install plugins to ${CMAKE_INSTALL_PREFIX}")
	  MESSAGE("Will install shared files to ${CMAKE_SHARED_PREFIX}")
	ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
ENDIF(NOT DEFINED RERUN)
	



# add a doc target to generate API information with doxygen
find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen.conf ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation" VERBATIM)
ENDIF(DOXYGEN_FOUND)

if(OPENMP_FOUND)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -pipe -fPIC -DPIC")

add_subdirectory(external)
add_subdirectory(src)




