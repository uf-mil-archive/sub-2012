# This file contains CMake functions that act as standard project templates.
# Instead of defining every sub-project in an independent and ad-hoc manner,
# most of them should be able to be defined by only calling one or two of the functions
# from this file. This also means that changes to the underlying mechanics of the build
# process can be kept isolated to this one file, while the rest of our cmake code
# is a simple definition of the desired project layout.

# Set up variables used in config.h.in files
# There may be a better way to do this but I can't find it
if(CMAKE_CROSSCOMPILING)
	file(RELATIVE_PATH SUBJUGATOR_RUNTIME_INSTALL_PREFIX ${CMAKE_FIND_ROOT_PATH} ${CMAKE_INSTALL_PREFIX}) # Strip the find root path from the install prefix to get the runtime path of the installed binaries
	set(RELATIVE_PATH "/${RELATIVE_PATH}")
else()
	set(SUBJUGATOR_RUNTIME_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()
set(SUBJUGATOR_CONFIG_DIRECTORY etc/subjugator CACHE STRING "Where the configuration files for the various subjugator binaries will be placed, relative to the install prefix")

#############################################
# sub_executable(ProjectName [DDS] [Qt])
# Configures this project to build and install an executable, which is always
# the name of the project in lowercase. If the project depends on DDS,
# the DDS flag must be placed after the project name. All DDS generated code is placed in a
# static library, which can be linked to other executables that need to communicate
# using datatypes defined in this project. See sub_reference_executable.
#
# Executable projects can also link to library projects. See sub_reference_library.
#
# Project layout:
#   CMakeLists.txt - contains a call to this function, as well as to sub_reference_executable
#   idl/<ProjectName>/ - contains .idl files defining DDS data types
#   include/<ProjectName>/ - contains header files
#   source/ - contains source files.
#
# It is suggested to place all idl files and header files in a subdirectory named after the project,
# so that when other projects include these files, the path will give away to which project they belong:
#   #include <HAL/Transport.h>
#   #include <IMUSensor/Sensors.h>
# versus
#   #include <Transport.h>
#   #include <Sensors.h>
#
# The path to the IDL file determines the path to the generated DDS headers. ex.
# IDL file idl/Thruster/ThrusterOutput.idl generates Thruster/ThrusterOutput.h,
# Thruster/ThrusterOutputSupport.h, etc. which can be included like normal:
#   #include <Thruster/ThrusterOutput.h>
#   #include <Thruster/ThrusterOutputSupport.h>
# These generated headers are placed in the build directory, not the source directory,
# to avoid polluting the version controlled source code with non-version controlled generated code,
# and to allow different builds to alter code generation settings and pull from the same source.
#############################################

function(sub_executable projectname)
	list(FIND ARGN DDS dds_pos) #determine if the DDS flag was given
	if (dds_pos EQUAL -1)
		set(dds FALSE)
	else()
		set(dds TRUE)
	endif()

	if (dds AND NOT NDDS_FOUND)
		message(ERROR "sub_executable called with dds enabled, but DDS was not found")
		return()
	endif()

	list(FIND ARGN Qt qt_pos)
	if (qt_pos EQUAL -1)
		set(qt FALSE)
	else()
		set(qt TRUE)
	endif()

	if (qt AND NOT QT_FOUND)
		message(ERROR "sub_executable called with qt enabled, but QT was not found")
		return()
	endif()

	list(FIND ARGN OpenCV cv_pos)
	if(cv_pos EQUAL -1)
		set(cv FALSE)
	else()
		set(cv TRUE)
	endif()

	list(FIND ARGN FlyCapture flycapture_pos)
	if(flycapture_pos EQUAL -1)
		set(flycapture FALSE)
	else()
		set(flycapture TRUE)
	endif()

	list(FIND ARGN GSL gsl_pos)
	if(gsl_pos EQUAL -1)
		set(gsl FALSE)
	else()
		set(gsl TRUE)
	endif()

	project(${projectname})

	# set up some variables and includes
	file(GLOB_RECURSE sources "source/*.cpp")
	include_directories(${CMAKE_CURRENT_BINARY_DIR}) # generated headers are tossed in here by qt and others
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
		include_directories(include)
		file(GLOB_RECURSE headers "include/*.h")
	else()
		set(headers "")
	endif()
	string(TOLOWER ${projectname} exename)

	if(FFTW_FOUND)
		set(libraries ${libraries} ${FFTW_LIBRARIES})
	endif()

	# Optionally process the config header
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in) # if the project has one
		configure_file(include/config.h.in config.h) # configure it, putting it in the output directory
	endif()

	# Optionally set up dds functionality
	if(dds)
		include_directories(${NDDS_INCLUDE_DIRS}) # put DDS on the include path
		set(libraries ${libraries} ${NDDS_LIBRARIES}) # link to DDS
	endif()

	#optionally set up qt functionality
	if(qt)
		include(${QT_USE_FILE})
		if (QWT_FOUND)
		    include_directories(${QWT_INCLUDE_DIR}) # put QWT on the include path
		    set(libraries ${libraries} ${QWT_LIBRARIES})
		endif()
		set(libraries ${libraries} ${QT_LIBRARIES})

		QT4_WRAP_CPP(headers_moc_sources ${headers})
		set(sources ${sources} ${headers_moc_sources})

		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/forms)
			file(GLOB_RECURSE forms "forms/*.ui")
			QT4_WRAP_UI(forms_moc_sources ${forms})
			set(sources ${sources} ${forms_moc_sources})
		endif()

		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
			file(GLOB_RECURSE resources "resources/*.qrc")
			QT4_ADD_RESOURCES(resources_moc_sources ${resources})
			set(sources ${sources} ${resources_moc_sources})
		endif()
	endif()

	# OpenCV functionality
	if(cv)
		set(libraries ${libraries} ${OpenCV_LIBS})
		include_directories(${OpenCV_INCLUDE_DIRS})
	endif()

	# FlyCapture
	if(flycapture)
		set(libraries ${libraries} ${FLYCAPTURE_LIBRARIES})
		include_directories(${FLYCAPTURE_INCLUDES})
	endif()

	# GSL
	if(gsl)
		set(libraries ${GSL_LIBRARIES})
		include_directories(${GSL_INCLUDE_DIRS})
	endif()

	# Boost comes last, since flycapture depends on it
	set(libraries ${libraries} ${Boost_LIBRARIES})

	# Define executable
	add_executable(${exename} ${sources})
	target_link_libraries(${exename} ${libraries})

	# install
	set_property(TARGET ${exename} PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE) # this causes CMake to keep runtime paths in the binary so dds libs are still found
	install(TARGETS ${exename} DESTINATION bin)

	# install configs
	file(GLOB_RECURSE configs "config/*")
	install(FILES ${configs} DESTINATION ${SUBJUGATOR_CONFIG_DIRECTORY})
endfunction()

#############################################
# sub_library(ProjectName)
# Configures this project to build a static library, which can be linked into
# executable projects. See sub_reference_library.
#
# Project Layout:
#   CMakeLists.txt
#   include/<ProjectName>/ - contains header files. will be seen by referencing projects
#   source/ - contains source files
#############################################

function(sub_library projectname)
	project(${projectname})

	include_directories(include)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})

	# Process the config header
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in) # if the project has one
		configure_file(include/config.h.in config.h) # configure it, putting it in the output directory
	endif()

	file(GLOB_RECURSE sources "source/*.cpp")
	string(TOLOWER ${projectname} libname)
	add_library(${libname} ${sources})

	# install configs
	file(GLOB_RECURSE configs "config/*")
	install(FILES ${configs} DESTINATION ${SUBJUGATOR_CONFIG_DIRECTORY})
endfunction()

#############################################
# sub_reference_library(ProjectName ReferencedLibraryProject1 ReferencedLibraryProject2 ...)
#
# This allows an executable project to use libraries created in library projects.
# It does so by placing the libraries' include directories in the include path for ProjectName,
# and linking the static libraries with the executable generated by ProjectName.
#############################################

function(sub_reference_library projectname)
	string(TOLOWER ${projectname} exename)

	foreach(refprojectname ${ARGN})
		string(TOLOWER ${refprojectname} reflibname)
		target_link_libraries(${exename} ${reflibname})
		include_directories(${${refprojectname}_SOURCE_DIR}/include)
	endforeach()
endfunction()

#############################################
# sub_lirbary_reference_library(ProjectName ReferencedLibraryProject1 ReferencedLibraryProject2 ...)
#
# This allows a library project to use headers defined in another library project.
#############################################

function(sub_library_reference_library projectname)
	foreach(refprojectname ${ARGN})
		string(TOLOWER ${refprojectname} reflibname)
		include_directories(${${refprojectname}_SOURCE_DIR}/include)
	endforeach()
endfunction()

# Apparently -fPIC is recommended for any build on 64 bit linux.
if (NOT CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	if (CMAKE_COMPILER_IS_GNUCXX)
		add_definitions(-fPIC)
	else()
		message(WARNING "Don't know how to force -fPIC on x64 with this compiler") # don't feed -fPIC to somebody other than GCC, it'll probably cause errors
	endif()
endif()

