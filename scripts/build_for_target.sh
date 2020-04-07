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

source $1/scripts/target_build_config.sh $1 $2

source $AQUABOX_ROOT/scripts/cleanup.sh

export SYSROOT_DIR=$AQUABOX_ROOT/target/rootfs

sudo ln -s $SYSROOT_DIR/lib/arm-linux-gnueabihf /lib/arm-linux-gnueabihf
sudo ln -s $SYSROOT_DIR/usr/lib/arm-linux-gnueabihf /usr/lib/arm-linux-gnueabihf

source $AQUABOX_ROOT/scripts/set_tools.sh
source $AQUABOX_ROOT/scripts/cmake_for_target.sh
