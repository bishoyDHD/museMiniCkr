# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(allyoucaneat)

SET(INCLUDES ${ROOT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/src/include" "${CMAKE_SOURCE_DIR}/include")

INCLUDE_DIRECTORIES(${INCLUDES})

SET(allyoucaneat_SOURCES src/allyoucaneat.cpp )
SET(allyoucaneat_HEADERS
${CMAKE_CURRENT_SOURCE_DIR}/src/include/allyoucaneat.h)

SET(michelin_SOURCES src/michelin.cpp src/comparewindow.cpp src/TSuperClasses.cc)
SET(michelin_HEADERS
${CMAKE_CURRENT_SOURCE_DIR}/src/include/comparewindow.h)


# create root dictionary
SET(LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/include/LinkDef.hh)
SET(DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/allyoucaneatDict.cpp)
ROOT_GENERATE_DICTIONARY("${allyoucaneat_HEADERS}" "${LINKDEF}" "${DICTIONARY}" "${INCLUDES}")
LIST(APPEND allyoucaneat_SOURCES ${DICTIONARY})

add_executable(allyoucaneat ${allyoucaneat_SOURCES})


SET(mLINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/include/mLinkDef.hh)
SET(mDICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/michelinDict.cpp)
ROOT_GENERATE_DICTIONARY("${michelin_HEADERS}" "${mLINKDEF}" "${mDICTIONARY}" "${INCLUDES}")
LIST(APPEND michelin_SOURCES ${mDICTIONARY})


TARGET_LINK_LIBRARIES(allyoucaneat Chef "-L${ROOT_LIBRARY_DIR}" ${ROOT_LIBRARIES} ${Boost_LIBRARIES} )


add_executable(michelin ${michelin_SOURCES})

TARGET_LINK_LIBRARIES(michelin Chef "-L${ROOT_LIBRARY_DIR}" ${ROOT_LIBRARIES}  ${Boost_LIBRARIES} )


INSTALL(TARGETS allyoucaneat RUNTIME DESTINATION ${BINDIR} LIBRARY
DESTINATION ${LIBDIR} ARCHIVE DESTINATION ${LIBDIR})

INSTALL(TARGETS michelin RUNTIME DESTINATION ${BINDIR} LIBRARY DESTINATION ${LIBDIR} ARCHIVE DESTINATION ${LIBDIR})

