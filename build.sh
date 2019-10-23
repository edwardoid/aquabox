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


echo "Required CMAKE is 3.7.2"
#export CMAKE=$HOME/cmake-3.7.2/bin/cmake
export CMAKE=cmake
export SRC_DIR=$PWD/../..
echo "Toolchain should be installed in /usr"
source $SRC_DIR/setup-arm-dev-env.sh
#rm -rf *
$CMAKE $SRC_DIR -DCMAKE_BUILD_TYPE=Release -DTARGET_DEVICE=YES -DCMAKE_TOOLCHAIN_FILE=$SRC_DIR/arm-gcc-toolchain.cmake
make -j8
#cp -r $SRC_DIR/cfg $PWD/devices
#cp $SRC_DIR/config.json $PWD/config.json
#cp startup.sh $PWD/startup.sh
