project(LegacyDDS)

include_directories(${NDDS_INCLUDE_DIRS})
file(GLOB_RECURSE interfaces "idl/*.idl")
ndds_run_rtiddsgen(interfaces_sources ${interfaces})
add_library(legacydds ${interfaces_sources})

