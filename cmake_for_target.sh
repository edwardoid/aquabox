#!/bin/bash

# AquaBox
# Copyright (C) 2019  Eduard Sargsyan
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


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
