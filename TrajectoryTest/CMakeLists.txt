sub_executable(TrajectoryTest Qt)
sub_reference_library(TrajectoryTest SubMain DataObjects HAL)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(QtApp_RCC_SRCS ${QtApp_RCCS})
# generate rules for building source files that moc generates
QT4_WRAP_CPP(QtApp_MOC_SRCS ${QtApp_MOCS})