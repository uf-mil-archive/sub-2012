# This file contains CMake functions that act as standard project templates.
# Instead of defining every sub-project in an independent and ad-hoc manner,
# most of them should be able to be defined by only calling one or two of the functions
# from this file. This also means that changes to the underlying mechanics of the build
# process can be kept isolated to this one file, while the rest of our cmake code
# is a simple definition of the desired project layout.

#############################################
# sub_executable(ProjectName)
# Configures this project to build and install an executable, which is always
# the name of the project in lowercase. All DDS generated code is placed in a
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
	project(${projectname})

	# includes
	include_directories(include)
	ndds_include_rtiddsgen_directories(idl)

	# Sources
	file(GLOB_RECURSE interfaces "idl/*.idl")
	file(GLOB_RECURSE sources "source/*.cpp")

	# Executable
	string(TOLOWER ${projectname} exename)
	add_executable(${exename} ${sources})
	target_link_libraries(${exename} ${Boost_LIBRARIES} ${NDDS_LIBRARIES})

	# DDS Library (optional)
	if (interfaces)
		set(ddslibname ${exename}_ddslib)
		ndds_run_rtiddsgen(interfaces_sources ${interfaces})
		add_library(${ddslibname} ${interfaces_sources})

		target_link_libraries(${exename} ${ddslibname})
	endif()

	# install
	set_property(TARGET ${exename} PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE) # this causes CMake to keep runtime paths in the binary so dds libs are still found
	install(TARGETS ${exename} DESTINATION bin)
endfunction()

#############################################
# sub_reference_executable(ProjectName, ReferencedProject1, ReferencedProject2, ...)
#
# This allows an executable project to access DDS generated datatype code from other executable projects.
# It does so by placing the generated header files of each ReferencedProject in the include path of ProjectName,
# as well as linking the binary from ProjectName with each of the DDS generated shared libraries of the
# referenced projects.
#
# When possible, an executable which publishes a datatype should contain its IDL, and any executable
# which subscribes to it should reference it.
#############################################

function(sub_reference_executable projectname)
	string(TOLOWER ${projectname} exename)

	foreach(refprojectname ${ARGN})
		string(TOLOWER ${refprojectname} refexename)
		set(refddslibname ${refexename}_ddslib)

		ndds_include_project_rtiddsgen_directories(${refprojectname} idl)
		target_link_libraries(${exename} ${refddslibname})
	endforeach()
endfunction()

