add_definitions(-ffast-math)

sub_executable(Hydrophone LegacyDDS)
sub_reference_library(Hydrophone HAL DataObjects SubMain)

