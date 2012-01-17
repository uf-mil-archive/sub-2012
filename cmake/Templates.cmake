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
	set(SUBJUGATOR_RUNTIME_INSTALL_PREFIX "/${SUBJUGATOR_RUNTIME_INSTALL_PREFIX}")
else()
	set(SUBJUGATOR_RUNTIME_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()
set(SUBJUGATOR_CONFIG_DIRECTORY etc/subjugator CACHE STRING "Where the configuration files for the various subjugator binaries will be placed, relative to the install prefix")

#############################################
# sub_executable(ProjectName [LegacyDDS] [Qt] [OpenCV] [FlyCapture] ReferencedLibraryProject1 ReferencedLibraryProject2 ...)
# Configures this project to build and install an executable, which is always
# the name of the project in lowercase. If the project depends on DDS,
# the DDS flag must be placed after the project name. All DDS generated code is placed in a
# static library, which can be linked to other executables that need to communicate
# using datatypes defined in this project. See sub_reference_executable.
#
# ReferencedLibraryProject(s) allow an executable project to use libraries created in library projects.
# They do so by placing the libraries' include directories in the include path,
# and linking the static libraries with the executable.
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

	# find sources and headers
	file(GLOB_RECURSE sources "source/*.cpp")
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
		include_directories(include)
		file(GLOB_RECURSE headers "include/*.h")
	else()
		set(headers "")
	endif()

	# Optionally process the config header
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in) # if the project has one
		include_directories(${CMAKE_CURRENT_BINARY_DIR})
		configure_file(include/config.h.in config.h) # configure it, putting it in the output directory
	endif()

	# Process each reference
	set(have_dds false)
	foreach(ref ${ARGN})
		if(ref STREQUAL LegacyDDS OR ref STREQUAL DDS)
			include_directories(${NDDS_INCLUDE_DIRS}) # put DDS on the include path
			set(libraries ${libraries} ${NDDS_LIBRARIES}) # link to DDS
			set(have_dds true)
		endif()

		if(ref STREQUAL LegacyDDS)
			ndds_include_project_rtiddsgen_directories(LegacyDDS C++ idl) # Put our DDS project's IDLs on the include path
			include_directories(${LegacyDDS_SOURCE_DIR}/include) # Put its regular headers on the include path
			set(libraries ${libraries} legacydds) # link to its libraries

		elseif(ref STREQUAL DDS)
			ndds_include_project_rtiddsgen_directories(DDS C++ idl) # Put our DDS project's IDLs on the include path
			include_directories(${DDS_SOURCE_DIR}/include) # Put its regular headers on the include path
			set(libraries ${libraries} dds) # link to its libraries

		elseif(ref STREQUAL Qt)
			set(QT_USE_QTOPENGL TRUE)
			include(${QT_USE_FILE})
			include_directories(${CMAKE_CURRENT_BINARY_DIR})
			set(libraries ${libraries} ${QT_LIBRARIES})

			if (QWT_FOUND)
				include_directories(${QWT_INCLUDE_DIR}) # put QWT on the include path
				set(libraries ${libraries} ${QWT_LIBRARIES})
			endif()

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

		elseif(ref STREQUAL OpenCV)
			set(libraries ${libraries} ${OpenCV_LIBS})
			include_directories(${OpenCV_INCLUDE_DIRS})

		elseif(ref STREQUAL FlyCapture)
			set(libraries ${libraries} ${FLYCAPTURE_LIBRARIES})
			include_directories(${FLYCAPTURE_INCLUDES})

		elseif(${ref}_SOURCE_DIR)
			string(TOLOWER ${ref} libname)
			set(libraries ${libraries} ${libname})
			include_directories(${${ref}_SOURCE_DIR}/include)
		else()
			message(SEND_ERROR "Executable ${projectname} has unknown reference ${ref}")
		endif()
	endforeach()

	# automatically link to FFTW if we found it
	if(FFTW_FOUND)
		set(libraries ${libraries} ${FFTW_LIBRARIES})
	endif()

	# Boost comes last, since flycapture depends on it
	set(libraries ${libraries} ${Boost_LIBRARIES} pthread rt)

	# Define executable
	string(TOLOWER ${projectname} exename)
	add_executable(${exename} ${sources})
	target_link_libraries(${exename} ${libraries})

	# this causes CMake to keep runtime paths in the binary so dds libs are still found
	# However, we're only doing this for LegacyDDS binaries. For new code you've got to set up ldconfig to find DDS libraries correctly
	list(FIND ARGN LegacyDDS pos)
	if(NOT pos EQUAL -1)
		set_property(TARGET ${exename} PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
	endif()

	# install
	install(TARGETS ${exename} DESTINATION bin)

	# install configs
	install(DIRECTORY config/ DESTINATION ${SUBJUGATOR_CONFIG_DIRECTORY} FILES_MATCHING PATTERN "*.json")
endfunction()

#############################################
# sub_library(ProjectName [DDS] ReferencedProjects...)
# Configures this project to build a static library, which can be linked into
# executable projects. Additional arguments allow a library project to use
# headers defined in another library project.
#
# Project Layout:
#   CMakeLists.txt
#   include/<ProjectName>/ - contains header files. will be seen by referencing projects
#   source/ - contains source files
#############################################

function(sub_library projectname)
	project(${projectname})

	file(GLOB_RECURSE sources "source/*.cpp")
	include_directories(include)

	# Process the config header
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in) # if the project has one
		include_directories(${CMAKE_CURRENT_BINARY_DIR})
		configure_file(include/config.h.in config.h) # configure it, putting it in the output directory
	endif()

	foreach(ref ${ARGN})
		if(ref STREQUAL DDS)
			include_directories(${NDDS_INCLUDE_DIRS} ${DDS_SOURCE_DIR})

		elseif(${ref}_SOURCE_DIR)
			include_directories(${${ref}_SOURCE_DIR}/include)

		else()
			message(SEND_ERROR "Library ${projectname} has unknown reference ${ref}")
		endif()
	endforeach()

	string(TOLOWER ${projectname} libname)
	add_library(${libname} ${sources})

	# install configs
	file(GLOB_RECURSE configs "config/*")
	install(FILES ${configs} DESTINATION ${SUBJUGATOR_CONFIG_DIRECTORY})
endfunction()
