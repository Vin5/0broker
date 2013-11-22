cmake_minimum_required(VERSION 2.6)

project(0broker)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug")
endif()

add_definitions("-Wall -std=c++11 -pedantic")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions("-D_DEBUG")
endif()

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

file(GLOB ZBROKER_INCLUDES
    "include/*.hpp")

file(GLOB ZBROKER_SOURCES
    "src/*.cpp")

add_executable(zbroker ${ZBROKER_INCLUDES} ${ZBROKER_SOURCES})

target_link_libraries(zbroker ${ZeroMQ_LIBRARIES})
target_link_libraries(zbroker ${Boost_LIBRARIES})
