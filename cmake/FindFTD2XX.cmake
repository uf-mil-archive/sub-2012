find_path(FTD2XX_INCLUDES NAMES ftd2xx.h)
find_library(FTD2XX_LIBRARIES NAMES ftd2xx)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FTD2XX DEFAULT_MSG FTD2XX_INCLUDES FTD2XX_LIBRARIES)

