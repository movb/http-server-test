cmake_minimum_required(VERSION 2.6) 

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb3")
# set_property CXX_STANDARD 11 not working on some platforms
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES("${SOURCE_DIR}")

find_package( Boost REQUIRED COMPONENTS system thread)
include_directories( ${Boost_INCLUDE_DIRS} )

FILE(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp ${SOURCE_DIR}/*.hpp)

add_executable (server ${SOURCES})
target_link_libraries(server ${Boost_LIBRARIES} pthread)
set_property(TARGET server PROPERTY CXX_STANDARD 11)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
#target_include_directories (server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
