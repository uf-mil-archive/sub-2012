find_package(FTD2XX)

if(NOT FTD2XX_FOUND)
	message("ftd2xx not found, hydrophone will not be built")
	return()
endif()

sub_executable(Hydrophone DDS)
sub_reference_library(Hydrophone HAL DDSCommanders DDSListeners DDSMessages DataObjects SubMain)

include_directories(${FTD2XX_INCUDLES})
target_link_libraries(hydrophone ${FTD2XX_LIBRARIES})

