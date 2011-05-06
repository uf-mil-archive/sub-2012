cmake_minimum_required(VERSION 2.6)
project(SubjuGator)

# Ensure CMake will find our custom modules in the cmake directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Load our template functions
include(Templates)

# find boost
find_package(Boost REQUIRED COMPONENTS system thread date_time)
include_directories(${Boost_INCLUDE_DIRS})

# find DDS, but not REQUIRED
find_package(NDDS QUIET)
if(NOT NDDS_FOUND)
	message("DDS Not found! No projects depending on DDS will be built.")
endif()

# Add projects subdirectories here
# (maybe possible to find projects automatically?)

add_subdirectory(SubMain)

# Handle the example projects

set(BUILD_EXAMPLE_PROJECTS FALSE CACHE BOOL "Whether to build simple demonstration projects in ExampleProjects/")

if(BUILD_EXAMPLE_PROJECTS)
	if(NDDS_FOUND)
		add_subdirectory(ExampleProjects/DDSPublisher)
		add_subdirectory(ExampleProjects/DDSSubscriber)
	endif()

	add_subdirectory(ExampleProjects/SimpleLib)
	add_subdirectory(ExampleProjects/SimpleLibUser)
endif()

