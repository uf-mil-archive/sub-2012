cmake_minimum_required(VERSION 2.6)
project(SubjuGator)

# Ensure CMake will find our custom modules in the cmake directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Load our template functions
include(Templates)
include(UseDoxygen)

# find boost
find_package(Boost REQUIRED COMPONENTS system thread date_time regex)
include_directories(${Boost_INCLUDE_DIRS})

# find Eigen and FFTW
find_package(Eigen3 REQUIRED)
find_package(FFTW REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR} ${FFTW_INCLUDES})
add_definitions(-DEIGEN_FFTW_DEFAULT)

# find DDS, but not REQUIRED
find_package(NDDS)
if(NOT NDDS_FOUND)
	message("DDS Not found! No projects depending on DDS will be built.")
endif()

find_package(Qt4)
if(NOT QT_FOUND)
	message("QT Not found! No projects depending on QT will be built.")
endif()

# Turn on warnings
add_definitions(-Wall)

# Add projects subdirectories here
# (maybe possible to find projects automatically?)

add_subdirectory(SubMain)
add_subdirectory(Hydrophone)
add_subdirectory(HAL)
add_subdirectory(HALTest)
add_subdirectory(DataObjects)
add_subdirectory(MotorCalibrate)

# Handle the example projects

set(BUILD_EXAMPLE_PROJECTS FALSE CACHE BOOL "Whether to build simple demonstration projects in ExampleProjects/")

if(BUILD_EXAMPLE_PROJECTS)
	if(NDDS_FOUND)
		add_subdirectory(ExampleProjects/DDSPublisher)
		add_subdirectory(ExampleProjects/DDSSubscriber)
	endif()

	if(QT_FOUND)
		add_subdirectory(ExampleProjects/SimpleQt)
	endif()

	add_subdirectory(ExampleProjects/SimpleLib)
	add_subdirectory(ExampleProjects/SimpleLibUser)
endif()

