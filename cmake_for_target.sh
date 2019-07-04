#!/bin/bash

rm 3dp/ cmake_install.cmake CMakeCache.txt CMakeFiles  Makefile  -rf
cmake $PWD/../../ \
			-DCMAKE_TOOLCHAIN_FILE=$PWD/../../arm-gcc-toolchain.cmake \
			-DCMAKE_SYSTEM_LIBRARY_PATH=$SYSROOT_DIR/lib \
			-DCMAKE_SYSTEM_INCLUDE_PATH=$SYSROOT_DIR/usr/include \
			-DCMAKE_SYSTEM_PREFIX_PATH=$SYSROOT_DIR \
			-DCMAKE_SOURCE_DIR=$PWD/../src/service/ \
			-DTARGET_DEVICE=YES \
			-DCMAKE_BUILD_TYPE=Release \
			-DCMAKE_LIBRARY_PATH=$SYSROOT_DIR/usr/lib/arm-linux-gnueabihf \
			-DFREETYPE_INCLUDE_DIR_ft2build=$SYSROOT_DIR/usr/include/freetype2 \
			-DFREETYPE_INCLUDE_DIR_freetype2=$SYSROOT_DIR/usr/include/freetype2
