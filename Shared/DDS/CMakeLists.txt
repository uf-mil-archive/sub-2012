project(DDS)
include_directories(include ${NDDS_INCLUDE_DIRS})

file(GLOB_RECURSE interfaces "idl/*.idl")
file(GLOB_RECURSE sources "source/*.cpp")
ndds_run_rtiddsgen(interfaces_sources C++ ${interfaces})
add_library(dds ${interfaces_sources} ${sources})

if(BUILD_PC)
    ndds_run_rtiddsgen(interfaces_c_sources C ${interfaces})
    add_library(dds_c SHARED ${interfaces_c_sources})
    install(TARGETS dds_c DESTINATION lib)
endif()
