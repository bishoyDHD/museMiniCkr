# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

PROJECT(SurveyFitter CXX C Fortran)
find_package( BLAS REQUIRED )

find_package(MPI REQUIRED)



SET(INCLUDES "${CMAKE_SOURCE_DIR}/include" ${Boost_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/external/cminpack")
INCLUDE_DIRECTORIES(${INCLUDES} )


add_executable(SurveyFitter SurveyFitter.cc)
TARGET_LINK_LIBRARIES(SurveyFitter ${BLAS_LIBRARIES} cminpack)

