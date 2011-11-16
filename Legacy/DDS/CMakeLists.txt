project(LegacyDDS)

include_directories(${NDDS_INCLUDE_DIRS})
file(GLOB_RECURSE interfaces "idl/*.idl")
ndds_run_rtiddsgen(interfaces_sources ${interfaces})
add_library(legacydds ${interfaces_sources})

ndds_run_rtiddsgen_c(interfaces_c_sources ${interfaces})
add_library(ddsmessages_c SHARED ${interfaces_c_sources})
target_link_libraries(ddsmessages_c ${NDDS_LIBRARIES})
