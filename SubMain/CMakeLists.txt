project(SubMain)

include_directories(include)
include_directories(include/SubMain/Workers)
set(HEADER_FILES
	include/SubMain/Workers/SubWorker.h
	include/SubMain/SubThruster.h
)

set(SOURCE_FILES
	source/Workers/SubWorker.cpp
	source/SubThruster.cpp
)

add_library(SubMain ${HEADER_FILES} ${SOURCE_FILES})
