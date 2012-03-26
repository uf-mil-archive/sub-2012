cmake_minimum_required(VERSION 2.8)
project(SubjuGator)

# Ensure CMake will find our custom modules in the cmake directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Load our template functions
include(Templates)
include(UseDoxygen)
include(PythonUtils)

# Turn on warnings
add_definitions(-Wall)

# Handle build types
if(NOT SUB_BUILD_TYPE)
	set(SUB_BUILD_TYPE all CACHE STRING "The set of programs to build. all, pc, or gumstix")
endif()
if(${SUB_BUILD_TYPE} STREQUAL "pc")
	set(BUILD_PC TRUE)
	set(BUILD_GUMSTIX FALSE)
elseif(${SUB_BUILD_TYPE} STREQUAL "gumstix")
	set(BUILD_PC FALSE)
	set(BUILD_GUMSTIX TRUE)
else()
	set(BUILD_PC TRUE)
	set(BUILD_GUMSTIX TRUE)
endif()

# find required packages
find_package(NDDS REQUIRED)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

find_package(Boost 1.36 REQUIRED COMPONENTS system thread date_time regex program_options filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# add shared projects
add_subdirectory(Shared/HAL)
add_subdirectory(Shared/DDS)
add_subdirectory(Shared/LibSub)
add_subdirectory(Shared/WorkerManager)

# add legacy shared stuff
add_subdirectory(Legacy/HAL)
add_subdirectory(Legacy/DDS)
add_subdirectory(Legacy/DataObjects)
add_subdirectory(Legacy/SubMain)

# We're building LPOSVSS binaries for both systems until its running smoothly on the gumstix
add_subdirectory(Sensors/LPOSVSS)

# handle libraries and projects that only runs on the pc
if(BUILD_PC)
	find_package(Qt4)
	if(NOT QT_FOUND)
		message("QT Not found! No projects depending on QT will be built.")
	endif()

	find_package(QWT)
	if(NOT QWT_FOUND)
		message("QWT Not found! No projects depending on QWT will be build.")
	endif()

	find_package(OpenCV)
	if(NOT OpenCV_FOUND)
		message("OpenCV not found! No projects depending on OpenCV will be built")
	endif()

	find_package(FlyCapture)
	if(NOT FLYCAPTURE_FOUND)
		message("FlyCapture not found! No projects depending on FlyCapture will be built")
	endif()

	find_package(FFTW)
	if(FFTW_FOUND)
		include_directories(${FFTW_INCLUDES})
		add_definitions(-DEIGEN_FFTW_DEFAULT)
	else()
		message(WARNING "FFTW not found. Eigen will fall back to kissfft.")
	endif()

	add_subdirectory(Shared/Python)

	add_subdirectory(Sensors/Hydrophone)
	add_subdirectory(Mission/MissionPlanner)
	add_subdirectory(Control/PrimitiveDriver)
	add_subdirectory(Control/TrackingController)
	add_subdirectory(Control/TrajectoryGenerator)
	add_subdirectory(Control/C3Trajectory)
	add_subdirectory(Tools/SubLog)

	if(OpenCV_FOUND AND FLYCAPTURE_FOUND)
		add_subdirectory(Mission/Vision)
	endif()

	if(QT_FOUND)
		add_subdirectory(Tools/MotorCalibrate)
		add_subdirectory(Tools/DDSWatcher)
		add_subdirectory(Tools/PDTest)
		add_subdirectory(Tools/SubControl)
		if(QWT_FOUND)
			add_subdirectory(Tools/TrajectoryTest)
		endif()
	endif()
endif()

# handle projects that only run on the gumstix
if(BUILD_GUMSTIX)
	add_subdirectory(Sensors/IMU)
	add_subdirectory(Sensors/Depth)
	add_subdirectory(Sensors/DVL)
endif()

