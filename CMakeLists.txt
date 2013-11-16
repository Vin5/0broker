cmake_minimum_required(VERSION 2.6)

project(0broker)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_FLAGS "-std=c++0x -pedantic")

IF(NOT CMAKE_BUILD_TYPE)
        SET(CMAKE_BUILD_TYPE "Debug")
ENDIF()

find_package(ZeroMQ REQUIRED)

find_package(Boost 1.42.0 REQUIRED
            COMPONENTS
                program_options
                thread)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ZeroMQ_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

link_directories(${ZeroMQ_LIBRARY_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

file(GLOB ZBROKER_SOURCES
    "src/*.cpp")

add_executable(zbroker ${ZBROKER_SOURCES})

TARGET_LINK_LIBRARIES(zbroker ${ZeroMQ_LIBRARIES})
TARGET_LINK_LIBRARIES(zbroker ${Boost_LIBRARIES})