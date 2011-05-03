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

function(sub_reference_executable projectname)
	string(TOLOWER ${projectname} exename)

	# Project references
	foreach(refprojectname ${ARGN})
		string(TOLOWER ${refprojectname} refexename)
		set(refddslibname ${refexename}_ddslib)

		ndds_include_project_rtiddsgen_directories(${refprojectname} idl)
		target_link_libraries(${exename} ${refddslibname})
	endforeach()
endfunction()

