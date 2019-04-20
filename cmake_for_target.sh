#!/bin/bash

rm 3dp/ cmake_install.cmake CMakeCache.txt CMakeFiles  Makefile  -rf
cmake $PWD/../../	-DBoost_INCLUDE_DIRS=$SYSROOT_DIR/usr/include \
			-DCMAKE_SYSTEM_LIBRARY_PATH=$SYSROOT_DIR/lib/arm-linux-gnueabihf \
			-DCMAKE_SYSTEM_INCLUDE_PATH=$SYSROOT_DIR/usr/include/arm-linux-gnueabihf \
			-DCMAKE_LIBRARY_PATH=$SYSROOT_DIR/usr/lib/arm-linux-gnueabihf \
			-DCMAKE_SOURCE_DIR=$PWD/../src/service/ \
			-DBOOST_LIBRARYDIR=$SYSROOT_DIR/usr/lib/arm-linux-gnueabihf \
			-DPNG_PNG_INCLUDE_DIR=$SYSROOT_DIR/usr/include \
			-DZLIB_INCLUDE_DIR=$SYSROOT_DIR/usr/include/arm-linux-gnueabihf \
			-DTARGET_DEVICE=YES
