# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

enable_language (Fortran)
find_package( LAPACK REQUIRED )


# Set project name
PROJECT(CoilFitter)

SET(INCLUDES  ${Boost_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/external/cminpack"
"${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include" "${LAPACK_INCLUDE_DIR}")
INCLUDE_DIRECTORIES(${INCLUDES})

# Set output path for libraries and executables within build directory (makes linking easier...)
SET(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")


add_executable(coilfit coilfit.cc )
TARGET_LINK_LIBRARIES(coilfit ${LAPACK_LIBRARIES} cminpack )


