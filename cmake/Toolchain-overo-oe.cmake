set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER $ENV{OVEROTOP}/tmp/sysroots/x86_64-linux/usr/armv7a/bin/arm-angstrom-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER $ENV{OVEROTOP}/tmp/sysroots/x86_64-linux/usr/armv7a/bin/arm-angstrom-linux-gnueabi-g++)

set(CMAKE_FIND_ROOT_PATH $ENV{OVEROTOP}/tmp/sysroots/armv7a-angstrom-linux-gnueabi)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(NDDS_ARCH armv7leLinux2.6gcc4.4.1)
set(NDDS_HOME $ENV{OVEROTOP}/tmp/sysroots/armv7a-angstrom-linux-gnueabi/opt/RTI/ndds.4.5d)

set(SUB_BUILD_TYPE gumstix)
set(CMAKE_INSTALL_PREFIX /home/root)

