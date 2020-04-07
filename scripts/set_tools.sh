#/bin/bash

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


export CMAKE=cmake


source $AQUABOX_ROOT/scripts/target_build_config.sh $1 $2

### TOOLS ###
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export CC=$TOOLCHAIN_ROOT/bin/arm-linux-gnueabihf-gcc
export NM=$TOOLCHAIN_ROOT/bin/arm-linux-gnueabihf-nm
export LD=$TOOLCHAIN_ROOT/bin/arm-linux-gnueabihf-ld
export CXX=$TOOLCHAIN_ROOT/bin/arm-linux-gnueabihf-g++
export RANLIB=$TOOLCHAIN_ROOT/bin/arm-linux-gnueabihf-ranlib
export AR=$TOOLCHAIN_ROOT/bin/arm-linux-gnueabihf-ar
export CPP=$TOOLCHAIN_ROOT/bin/arm-linux-gnueabihf-cpp
export PKG_PROG_PKG_CONFIG=$TOOLCHAIN_ROOT/arm-linux-gnueabihf-pkg-config 
export LIBRARY_PATH=${LIBRARY_PATH}:${SYSROOT_DIR}/usr/lib/arm-linux-gnueabihf

### TOOLS ###

### PKG CONFIG ###
export PKG_CONFIG_SYSROOT_DIR=${SYSROOT_DIR}
PKG_CONFIG_PATH=${SYSROOT_DIR}/usr/arm-linux-gnueabi/usr/local/lib/pkgconfig
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${SYSROOT_DIR}/usr/lib/arm-linux-gnueabihf/pkgconfig/
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${SYSROOT_DIR}/usr/local/lib/pkgconfig
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${SYSROOT_DIR}/usr/local/share/pkgconfig
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${SYSROOT_DIR}/usr/lib/pkgconfig
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${SYSROOT_DIR}/usr/share/pkgconfigusr/
PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${SYSROOT_DIR}/usr/local/lib/pkgconfig
export PKG_CONFIG_PATH
### PKG CONFIG ###
