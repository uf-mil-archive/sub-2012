project(SubMain)

include_directories(include)
include_directories(include/Workers)
set(HEADER_FILES
	include/Workers/SubWorker.h
	include/SubThruster.h
)

set(SOURCE_FILES
	source/Workers/SubWorker.cpp
	source/SubThruster.cpp
)

add_library(SubMain ${HEADER_FILES} ${SOURCE_FILES})
