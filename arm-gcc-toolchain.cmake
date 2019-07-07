set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_SYSROOT ${CMAKE_CURRENT_LIST_DIR}/../arm/sysroot)
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

set(tools /usr)
set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


if (TARGET_DEVICE)
  add_definitions(-DTARGET_DEVICE)
  add_definitions(-DW1_BUS_ROOT="/sys/bus/w1/devices")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SYSROOT}/aquabox")
endif()
