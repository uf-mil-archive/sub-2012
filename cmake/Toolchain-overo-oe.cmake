set(GUMSTIX_ROOT_PATH $ENV{OVEROTOP}/tmp/sysroots/armv7a-angstrom-linux-gnueabi)
set(GUMSTIX_TOOLCHAIN_PATH $ENV{OVEROTOP}/tmp/sysroots/x86_64-linux/usr/armv7a/bin)
set(FLAGS "-O3 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -DNDEBUG")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER ${GUMSTIX_TOOLCHAIN_PATH}/arm-angstrom-linux-gnueabi-gcc)
set(CMAKE_C_FLAGS_RELEASE ${FLAGS} CACHE STRING "C compiler flags for gumstix release mode")
set(CMAKE_CXX_COMPILER ${GUMSTIX_TOOLCHAIN_PATH}/arm-angstrom-linux-gnueabi-g++)
set(CMAKE_CXX_FLAGS_RELEASE ${FLAGS} CACHE STRING "CXX compiler flags for gumstix release mode")

set(CMAKE_FIND_ROOT_PATH ${GUMSTIX_ROOT_PATH})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(NDDS_ARCH armv7leLinux2.6gcc4.4.1)
set(NDDS_HOME ${GUMSTIX_ROOT_PATH}/opt/RTI/ndds.4.5d)

set(SUB_BUILD_TYPE gumstix)
set(CMAKE_INSTALL_PREFIX ${GUMSTIX_ROOT_PATH}/home/root/sub)

