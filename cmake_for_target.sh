#!/bin/bash

rm 3dp/ cmake_install.cmake CMakeCache.txt CMakeFiles  Makefile  -rf
cmake $PWD/../../	-DBoost_INCLUDE_DIRS=$SYSROOT_DIR/usr/include \
			-DCMAKE_SYSTEM_LIBRARY_PATH=$SYSROOT_DIR/lib \
			-DCMAKE_SYSTEM_INCLUDE_PATH=$SYSROOT_DIR/usr/include \
			-DCMAKE_LIBRARY_PATH=$SYSROOT_DIR/usr/lib/arm-linux-gnueabihf \
			-DCMAKE_SOURCE_DIR=$PWD/../src/service/ \
			-DBOOST_LIBRARYDIR=$SYSROOT_DIR/usr/lib/arm-linux-gnueabihf \
			-DTARGET_DEVICE=YES \
			-DCMAKE_BUILD_TYPE=Release \
			-DFREETYPE_INCLUDE_DIR_ft2build=$SYSROOT_DIR/usr/include/freetype2 \
			-DFREETYPE_INCLUDE_DIR_freetype2=$SYSROOT_DIR/usr/include/freetype2 \
			-DLIBIW_LIBRARIES=$SYSROOT_DIR/usr/lib/arm-linux-gnueabihf/libiw.so
