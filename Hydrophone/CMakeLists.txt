add_definitions(-ffast-math)

sub_executable(Hydrophone DDS)
sub_reference_library(Hydrophone HAL DataObjects SubMain DDS)

