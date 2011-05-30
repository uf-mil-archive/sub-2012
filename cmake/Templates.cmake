# This file contains CMake functions that act as standard project templates.
# Instead of defining every sub-project in an independent and ad-hoc manner,
# most of them should be able to be defined by only calling one or two of the functions
# from this file. This also means that changes to the underlying mechanics of the build
# process can be kept isolated to this one file, while the rest of our cmake code
# is a simple definition of the desired project layout.

set(SUBJUGATOR_CONFIG_DIRECTORY etc/subjugator CACHE STRING "Where the configuration files for the various subjugator binaries will be placed, relative to the install prefix")

#############################################
# sub_executable(ProjectName [DDS])
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

	project(${projectname})

	# Process the config header
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in) # if the project has one
		configure_file(include/config.h.in config/config.h) # configure it, putting it in the output directory
		include_directories(${CMAKE_CURRENT_BINARY_DIR}/config) # put the output directory on the include path
	endif()

	# Define executable
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
		include_directories(include)
	endif()
	file(GLOB_RECURSE sources "source/*.cpp")
	string(TOLOWER ${projectname} exename)
	add_executable(${exename} ${sources})
	target_link_libraries(${exename} ${Boost_LIBRARIES} ${FFTW_LIBRARIES})

	# Optionally set up dds functionality as well
	if(dds)
		include_directories(${NDDS_INCLUDE_DIRS}) # put DDS on the include path
		target_link_libraries(${exename} ${NDDS_LIBRARIES}) # link to DDS

		file(GLOB_RECURSE interfaces "idl/*.idl") # build a shared lib to hold DDS generated code if we have any idl files
		if (interfaces)
			set(ddslibname ${exename}_ddslib)
			ndds_run_rtiddsgen(interfaces_sources ${interfaces})
			add_library(${ddslibname} ${interfaces_sources})
			target_link_libraries(${exename} ${ddslibname}) # link the library to our binary
			ndds_include_rtiddsgen_directories(idl) # put the directory containing headers generated with rtiddsgen on the include path
		endif()
	endif()

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

	file(GLOB_RECURSE sources "source/*.cpp")

	string(TOLOWER ${projectname} libname)
	add_library(${libname} ${sources})
endfunction()

#############################################
# sub_reference_executable(ProjectName ReferencedProject1 ReferencedProject2 ...)
#
# This allows an executable project to access DDS generated datatype code from other executable projects.
# It does so by placing the generated header files of each ReferencedProject in the include path of ProjectName,
# as well as linking the binary from ProjectName with each of the DDS generated shared libraries of the
# referenced projects.
#
# When possible, an executable which publishes a datatype should contain any associated IDL, and any executable
# which subscribes to topics using that datatype should reference it.
#############################################

function(sub_reference_executable projectname)
	if(NOT NDDS_FOUND)
		message(ERROR "sub_reference_executable called, but DDS not found")
	endif()

	string(TOLOWER ${projectname} exename)

	foreach(refprojectname ${ARGN})
		string(TOLOWER ${refprojectname} refexename)
		set(refddslibname ${refexename}_ddslib)

		ndds_include_project_rtiddsgen_directories(${refprojectname} idl)
		target_link_libraries(${exename} ${refddslibname})
	endforeach()
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

# Apparently -fPIC is recommended for any build on 64 bit linux.
if (NOT CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	if (CMAKE_COMPILER_IS_GNUCXX)
		add_definitions(-fPIC)
	else()
		message(WARNING "Don't know how to force -fPIC on x64 with this compiler") # don't feed -fPIC to somebody other than GCC, it'll probably cause errors
	endif()
endif()

