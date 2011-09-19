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
find_package(FFTW QUIET)
include_directories(${EIGEN3_INCLUDE_DIR})
if(FFTW_FOUND)
	include_directories(${FFTW_INCLUDES})
	add_definitions(-DEIGEN_FFTW_DEFAULT)
endif()

# find DDS, but not REQUIRED
find_package(NDDS)
if(NDDS_FOUND)
	add_subdirectory(DDSMessages)
	ndds_include_project_rtiddsgen_directories(DDSMessages idl)
else()
	message("DDS Not found! No projects depending on DDS will be built.")
endif()

find_package(Qt4)
if(NOT QT_FOUND)
	message("QT Not found! No projects depending on QT will be built.")
endif()

find_package(QWT)
if(NOT QWT_FOUND)
	message("QWT Not found! No projects depending on QWT will be build.")
endif()

#find OPENCV not required
find_package(OpenCV)
if(NOT OpenCV_FOUND)
	message("OpenCV not found! No projects depending on OpenCV will be built")
endif()

find_package(FlyCapture)
if(NOT FLYCAPTURE_FOUND)
	message("FlyCapture not found! No projects depending on FlyCapture will be built")
endif()

find_package(GSL)
if(NOT GSL_FOUND)
	message("GSL not found! Vision cannot be built!")
endif()

# Turn on warnings
add_definitions(-Wall)

# Handle build types
set(SUB_BUILD_TYPE all CACHE STRING "The set of programs to build. all, sub, or gumstix")

if(${SUB_BUILD_TYPE} STREQUAL "sub")
	set(BUILD_SUB TRUE)
	set(BUILD_GUMSTIX FALSE)
elseif(${SUB_BUILD_TYPE} STREQUAL "gumstix")
	set(BUILD_SUB FALSE)
	set(BUILD_GUMSTIX TRUE)
else()
	set(BUILD_SUB TRUE)
	set(BUILD_GUMSTIX TRUE)
endif()

# Add projects subdirectories here

add_subdirectory(HAL)
add_subdirectory(DataObjects)
add_subdirectory(SubMain)

if(NDDS_FOUND)
	add_subdirectory(DDSListeners)
	add_subdirectory(DDSCommanders)
endif()

if(BUILD_SUB)
	add_subdirectory(Hydrophone)
	add_subdirectory(MissionPlanner)
	add_subdirectory(PrimitiveDriver)
	if(OpenCV_FOUND AND FLYCAPTURE_FOUND AND GSL_FOUND)
		add_subdirectory(Vision)
	endif()
	if(QT_FOUND)
		add_subdirectory(MotorCalibrate)
		add_subdirectory(DepthTest)
		add_subdirectory(DDSWatcher)
		#add_subdirectory(SuckySim)
		if(QWT_FOUND)
			#add_subdirectory(TrajectoryTest)
		endif()
	endif()
	if(NDDS_FOUND)
		add_subdirectory(SensorLogger)
	endif()
	if(QT_FOUND AND NDDS_FOUND)
		add_subdirectory(PDTest)
	endif()
endif()

if(BUILD_GUMSTIX)
	if(NDDS_FOUND)
		add_subdirectory(IMUSmartSensor)
		add_subdirectory(DepthSmartSensor)
		add_subdirectory(DVLSmartSensor)
		add_subdirectory(LPOSVSS)
		add_subdirectory(TrackingController)
		add_subdirectory(TrajectoryGenerator)
		add_subdirectory(EquTrajectoryGenerator)
	endif()
endif()

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

