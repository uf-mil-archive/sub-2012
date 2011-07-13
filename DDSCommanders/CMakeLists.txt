sub_library(DDSCommanders)

include_directories(${NDDS_INCLUDE_DIRS})
sub_library_reference_library(DDSCommanders SubMain Hydrophone)
sub_reference_library(DDSCommanders DataObjects DDSMessages HAL SubMain)
