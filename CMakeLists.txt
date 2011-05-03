cmake_minimum_required(VERSION 2.6)
project(Subjugator)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(Templates)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(NDDS REQUIRED)
include_directories(${NDDS_INCLUDE_DIRS})

add_subdirectory(DDSPublisher)
add_subdirectory(DDSSubscriber)

