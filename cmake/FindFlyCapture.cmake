find_path(FLYCAPTURE_INCLUDES NAMES FlyCapture2.h PATH_SUFFIXES flycapture)
find_library(FLYCAPTURE_LIBRARIES NAMES flycapture)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FlyCapture DEFAULT_MSG FLYCAPTURE_INCLUDES FLYCAPTURE_LIBRARIES)

