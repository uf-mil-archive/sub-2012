cmake_minimum_required(VERSION 2.6)
project(Subjugator)

#ensure CMake will find our custom modules in the cmake directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(Templates)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(NDDS REQUIRED)
include_directories(${NDDS_INCLUDE_DIRS})

# TODO: Add a configuration option to disable building of example projects
add_subdirectory(ExampleProjects/DDSPublisher)
add_subdirectory(ExampleProjects/DDSSubscriber)
add_subdirectory(ExampleProjects/SimpleLib)
add_subdirectory(ExampleProjects/SimpleLibUser)

