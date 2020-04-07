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

$CMAKE	$AQUABOX_ROOT														\
		-DCMAKE_BUILD_TYPE=$BUILD_TYPE										\
		-DCMAKE_MODULE_PATH=$AQUABOX_ROOT/cmake 							\
		-DTARGET_DEVICE=YES													\
		-DCMAKE_TOOLCHAIN_FILE=$AQUABOX_ROOT/cmake/arm-gcc-toolchain.cmake