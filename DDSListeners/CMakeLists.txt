sub_library(DDSListeners)

include_directories(${NDDS_INCLUDE_DIRS})
sub_library_reference_library(DDSListeners SubMain)
sub_reference_library(DDSListeners DataObjects DDSMessages HAL SubMain)