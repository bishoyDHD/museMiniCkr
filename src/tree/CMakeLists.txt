#Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(musetree)


# Set the library version
SET(MUSETREE_MAJOR_VERSION 1)
SET(MUSETREE_MINOR_VERSION 1)
SET(MUSETREE_PATCH_VERSION 0)
SET(MUSETREE_VERSION "${MUSETREE_MAJOR_VERSION}.${MUSETREE_MINOR_VERSION}.${MUSETREE_PATCH_VERSION}")
LIST(APPEND MUSETREE_LIBRARY_PROPERTIES VERSION "${MUSETREE_VERSION}" SOVERSION "${MUSETREE_MAJOR_VERSION}")

# find source and header files, set paths, includes...
SET(LIBNAME "musetree")
FILE(GLOB SRCS src/*.cpp)

FILE(GLOB HEADERS src/include/*.h
${CMAKE_SOURCE_DIR}/include/gemControltree.h
${CMAKE_SOURCE_DIR}/include/muserawtree.h
${CMAKE_SOURCE_DIR}/include/museadctree.h 
${CMAKE_SOURCE_DIR}/include/musetdc1290tree.h 
${CMAKE_SOURCE_DIR}/include/musetdc1190tree.h 
${CMAKE_SOURCE_DIR}/include/musetrb3tree.h
${CMAKE_SOURCE_DIR}/include/musescalertree.h
${CMAKE_SOURCE_DIR}/include/lumigemtree.h 
${CMAKE_SOURCE_DIR}/include/teletracktree.h
${CMAKE_SOURCE_DIR}/include/generatortree.h
${CMAKE_SOURCE_DIR}/include/SciFitree.h
${CMAKE_SOURCE_DIR}/include/Trackhittree.h
${CMAKE_SOURCE_DIR}/include/trb3tree.h
${CMAKE_SOURCE_DIR}/include/v1190tree.h
${CMAKE_SOURCE_DIR}/include/v1290tree.h
${CMAKE_SOURCE_DIR}/include/v792tree.h
${CMAKE_SOURCE_DIR}/include/mqdctree.h
${CMAKE_SOURCE_DIR}/include/mappedchannelstree.h
${CMAKE_SOURCE_DIR}/include/BHtimetree.h
${CMAKE_SOURCE_DIR}/include/SciFiOutput.h
${CMAKE_SOURCE_DIR}/include/BMtree.h
${CMAKE_SOURCE_DIR}/include/ParTypestree.h
${CMAKE_SOURCE_DIR}/include/TRB_TDCtree.h

${CMAKE_SOURCE_DIR}/include/Hittree.h
${CMAKE_SOURCE_DIR}/include/TrackerTree.h

#${CMAKE_SOURCE_DIR}/include/SiPM.h

)

SET(INCLUDES ${ROOT_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/src/include" "${CMAKE_CURRENT_SOURCE_DIR}/../../include")
INCLUDE_DIRECTORIES(${INCLUDES})

# create root dictionary
SET(LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/include/LinkDef.hh)
SET(DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/${LIBNAME}Dict.cpp)
ROOT_GENERATE_DICTIONARY("${HEADERS}" "${LINKDEF}" "${DICTIONARY}" "${INCLUDES}")
LIST(APPEND SRCS ${DICTIONARY})

# build the library
ADD_LIBRARY(${LIBNAME} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${LIBNAME}  "-L${ROOT_LIBRARY_DIR}" ${ROOT_LIBRARIES})

SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES ${MUSETREE_LIBRARY_PROPERTIES})


# install the library
INSTALL(TARGETS ${LIBNAME} RUNTIME DESTINATION ${BINDIR} LIBRARY DESTINATION ${LIBDIR} ARCHIVE DESTINATION ${LIBDIR})

IF(INSTALL_HEADER)
	INSTALL(FILES ${HEADERS} DESTINATION ${INCDIR})
ENDIF(INSTALL_HEADER)

