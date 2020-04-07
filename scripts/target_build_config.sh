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


export AQUABOX_ROOT=$1
export TOOLCHAIN_ROOT=$2
export BUILD_TYPE=Release
export TARGET_SYSTEM=stretch
export CMAKE=cmake
export NCPU=`cat /proc/cpuinfo | grep vendor_id | wc -l`
export TARGET_IP=192.168.1.143
export TARGET_USER=root
