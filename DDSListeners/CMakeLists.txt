sub_library(DDSListeners)

include_directories(${NDDS_INCLUDE_DIRS})
sub_reference_library(DDSListeners SubMain DataObjects DDSMessages HAL)
