set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT ${CMAKE_CURRENT_LIST_DIR}/../arm/sysroot)
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

set(tools ${CMAKE_CURRENT_LIST_DIR}/../arm/toolchain)
set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabihf-g++)


set(ENV{PKG_CONFIG_PATH} "/usr/share/arm-linux-gnueabihf/pkgconfig:${CMAKE_SYSROOT}/usr/lib/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_SYSROOT}")
set(PKG_CONFIG_EXECUTABLE "/usr/bin/arm-linux-gnueabihf-pkg-config")
set(ENV{LD_LIBRARY_PATH} "${CMAKE_SYSROOT}/usr/lib")
set(ENV{C_INCLUDE_PATH} "${CMAKE_SYSROOT}/usr/include")
set(ENV{CPLUS_INCLUDE_PATH} "${CMAKE_SYSROOT}/usr/include")

include_directories("${CMAKE_SYSROOT}/usr/include/arm-linux-gnueabihf")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SYSROOT}/aquabox")
