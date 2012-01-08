project(DDS)
include_directories(include ${NDDS_INCLUDE_DIRS})

file(GLOB_RECURSE interfaces "idl/*.idl")
ndds_run_rtiddsgen(interfaces_sources ${interfaces})

file(GLOB_RECURSE sources "source/*.cpp")

add_library(dds ${interfaces_sources} ${sources})

