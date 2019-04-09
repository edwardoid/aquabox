#!/bin/bash

rm 3dp/ cmake_install.cmake CMakeCache.txt CMakeFiles  Makefile  -rf
echo $RASPB_SYSROOT
cmake $PWD/..	-DBoost_INCLUDE_DIRS=$RASPB_SYSROOT/usr/include \
		-DCMAKE_SYSTEM_LIBRARY_PATH=$RASPB_SYSROOT/lib/arm-linux-gnueabihf \
		-DCMAKE_SYSTEM_INCLUDE_PATH=$RASPB_SYSROOT/usr/include/arm-linux-gnueabihf \
		-DCMAKE_LIBRARY_PATH=$RASPB_SYSROOT/usr/lib/arm-linux-gnueabihf \
		-DCMAKE_SOURCE_DIR=$PWD/../src/service/ \
		-DBOOST_LIBRARYDIR=$RASPB_SYSROOT/usr/lib/arm-linux-gnueabihf \
		-DPNG_PNG_INCLUDE_DIR=$RASPB_SYSROOT/usr/include \
		-DZLIB_INCLUDE_DIR=$RASPB_SYSROOT/usr/include/arm-linux-gnueabihf \
