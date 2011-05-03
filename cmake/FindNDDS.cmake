# Platform detection and defines
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-DRTI_UNIX -DRTI_LINUX)
	set(NDDS_ARCH "x64Linux2.6gcc4.1.1" CACHE STRING "Architecture type for RTI DDS")
	mark_as_advanced(NDDS_ARCH)
else()
	message(FATAL_ERROR "TODO: FindNDDS.cmake windows support!")
endif()

# Root directory
find_path(NDDS_HOME rev_host_rtidds.4.5c HINTS $ENV{NDDSHOME} DOC "Root directory of RTI DDS")
if(NOT NDDS_HOME)
	set(ERR_MSG "${ERR_MSG}Failed to find root containing rev_host_rtidds.4.5c. ")
endif()

function(ndds_include_rtiddsgen_directories)
	foreach(DIR ${ARGN})
		include_directories("${CMAKE_CURRENT_BINARY_DIR}/rtiddsgen_out/${DIR}")
	endforeach()
endfunction()

function(ndds_include_project_rtiddsgen_directories PROJECTNAME)
	foreach(DIR ${ARGN})
		include_directories("${${PROJECTNAME}_BINARY_DIR}/rtiddsgen_out/${DIR}")
	endforeach()
endfunction()

# rtiddsgen
find_program(NDDS_RTIDDSGEN rtiddsgen HINTS ${NDDS_HOME}/scripts DOC "Path to rtiddsgen utility")
mark_as_advanced(NDDS_RTIDDSGEN)
if(NDDS_RTIDDSGEN)
	function(ndds_run_rtiddsgen OUTPUT_SOURCES_VARNAME)
		foreach(IDLFILE ${ARGN})
			set(OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/rtiddsgen_out)
			file(MAKE_DIRECTORY ${OUTDIR})

			file(RELATIVE_PATH IDLFILE_REL ${CMAKE_CURRENT_SOURCE_DIR} ${IDLFILE})
			set(SOURCENAME ${OUTDIR}/${IDLFILE_REL})
			string(REPLACE .idl "" SOURCENAME ${SOURCENAME})
			set(SOURCEFILES "${SOURCENAME}.cxx" "${SOURCENAME}Plugin.cxx" "${SOURCENAME}Support.cxx")

			get_filename_component(SOURCEFILE_DIR ${SOURCENAME} PATH)
			make_directory(${SOURCEFILE_DIR})

			add_custom_command(OUTPUT ${SOURCEFILES} DEPENDS "${IDLFILE}" COMMAND ${NDDS_RTIDDSGEN} -language C++ -inputIDL ${IDLFILE} -replace -d "${SOURCEFILE_DIR}")

			set(${OUTPUT_SOURCES_VARNAME} ${${OUTPUT_SOURCES_VARNAME}} ${SOURCEFILES} PARENT_SCOPE)
		endforeach()
	endfunction()
else()
	set(ERR_MSG "${ERR_MSG}Failed to find rtiddsgen binary. ")
endif()

# Include directory
find_path(NDDS_INCLUDE_DIR ndds/ndds_cpp.h HINTS ${NDDS_HOME}/include DOC "Include directory for RTI DDS")
mark_as_advanced(NDDS_INCLUDE_DIR)
if(NDDS_INCLUDE_DIR)
	set(NDDS_INCLUDE_DIRS ${NDDS_INCLUDE_DIRS} ${NDDS_INCLUDE_DIR} ${NDDS_INCLUDE_DIR}/ndds)
else()
	set(ERR_MSG "${ERR_MSG}Failed to include directory with ndds/ndds_cpp.h. ")
endif()

# Libraries
set(NDDS_LIBDIR_HINT "${NDDS_HOME}/lib/${NDDS_ARCH}")
foreach(LIB nddscpp nddsc nddscore)
	find_library(NDDS_LIB_${LIB}_RELEASE ${LIB} HINTS ${NDDS_LIBDIR_HINT} DOC "Path to RTI DDS library ${LIB}")
	find_library(NDDS_LIB_${LIB}_DEBUG ${LIB}d HINTS ${NDDS_LIBDIR_HINT} DOC "Path to RTI DDS library ${LIB} built for debugging")

	if(NDDS_LIB_${LIB}_RELEASE AND NDDS_LIB_${LIB}_DEBUG)
		set(NDDS_LIBRARIES ${NDDS_LIBRARIES} optimized ${NDDS_LIB_${LIB}_RELEASE} debug ${NDDS_LIB_${LIB}_DEBUG})
	elseif(NDDS_LIB_${LIB}_RELEASE)
		set(NDDS_LIBRARIES ${NDDS_LIBRARIES} ${NDDS_LIB_${LIB}_RELEASE})
		message(WARNING "Missing debug version of NDDS library ${LIB}")
	elseif(NDDS_LIB_${LIB}_DEBUG)
		set(NDDS_LIBRARIES ${NDDS_LIBRARIES} ${NDDS_LIB_${LIB}_DEBUG})
		message(WARNING "Missing release version of NDDS library ${LIB}")
	else()
		set(ERR_MSG "${ERR_MSG}Failed to find library ${LIB}. ")
	endif()

	mark_as_advanced(NDDS_LIB_${LIB}_RELEASE NDDS_LIB_${LIB}_DEBUG)
endforeach()
set(NDDS_LIBRARIES ${NDDS_LIBRARIES} dl)

# Error/Status handling
if(ERR_MSG)
	set(NDDS_FOUND FALSE CACHE INTERNAL "Whether RTI DDS was found or not" FORCE)
else()
	if(NDDS_FOUND)
		return()
	else()
		set(NDDS_FOUND TRUE CACHE INTERNAL "Whether RTI DDS was found or not" FORCE)
	endif()
endif()

if(NOT NDDS_FIND_QUIETLY)
	if(NDDS_FOUND)
		message(STATUS "Found RTI DDS")
	else()
		if(NDDS_FIND_REQUIRED)
			message(FATAL_ERROR "Failed to find RTI DDS component(s). ${ERR_MSG}")
		else()
			message(WARNING "Failed to find RTI DDS component(s). ${ERR_MSG}")
		endif()
	endif()
endif()

