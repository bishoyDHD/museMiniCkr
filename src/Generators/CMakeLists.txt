# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(Generators)


SET(INCLUDES ${ROOT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
${GSL_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES(${INCLUDES})

add_library(Generators src/base.cpp src/isotropic.cpp src/born.cpp src/fixed.cpp)

TARGET_LINK_LIBRARIES(Generators "-L${ROOT_LIBRARY_DIR}"  dl ${ROOT_LIBRARIES} ${GSL_LIBRARIES} )

