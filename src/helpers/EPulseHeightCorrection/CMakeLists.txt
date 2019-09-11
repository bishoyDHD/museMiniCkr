# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(EPulseHeightCorrection)

SET(INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include" ${ROOT_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${INCLUDES})

# find source and header files, set paths, includes...
SET(LIBNAME "EPulseHeightCorrection")

set(SRCS src/EPulseHeightCorrection.cpp)
set(HEADERS ${CMAKE_SOURCE_DIR}/include/helpers/EPulseHeightCorrection.h)

SET(INCLUDES ${ROOT_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/src/include" "${CMAKE_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES(${INCLUDES})

# create root dictionary
SET(LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/LinkDef.hh)
SET(DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/${LIBNAME}Dict.cpp)
ROOT_GENERATE_DICTIONARY("${HEADERS}" "${LINKDEF}" "${DICTIONARY}" "${INCLUDES}")
LIST(APPEND SRCS ${DICTIONARY})


add_library(${LIBNAME} SHARED ${SRCS} )
TARGET_LINK_LIBRARIES(${LIBNAME} ${ROOT_LIBRARIES})

INSTALL(TARGETS EPulseHeightCorrection RUNTIME DESTINATION ${BINDIR} LIBRARY DESTINATION ${LIBDIR} ARCHIVE DESTINATION ${LIBDIR})

add_executable(EPHCdemo src/EPHCdemo.cpp)
TARGET_LINK_LIBRARIES(EPHCdemo EPulseHeightCorrection musetree ${ROOT_LIBRARIES})


