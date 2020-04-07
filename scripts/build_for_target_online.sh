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

export SYSROOT_DIR=$AQUABOX_ROOT/target/rootfs_online

mkdir -p $SYSROOT_DIR
echo "checking if Target is mounted on sysroot ... CFG: ${TARGET_USER}@${TARGET_IP}"
if grep -qs "${SYSROOT_DIR}" /proc/mounts; then
    echo "Sysroot is mounted, skipping"
else
    echo "Mounting Target in sysroot..."
    sudo sshfs -o allow_other ${TARGET_USER}@${TARGET_IP}:/ ${SYSROOT_DIR}
fi


if [  -L  "/lib/arm-linux-gnueabihf" ]; then
      sudo rm /lib/arm-linux-gnueabihf
fi

if [ -L "/usr/lib/arm-linux-gnueabihf" ]; then
        sudo rm /usr/lib/arm-linux-gnueabihf
fi

if [ ! -d "/lib/arm-linux-gnueabihf" ]; then
    sudo mkdir /lib/arm-linux-gnueabihf
fi

if [ ! -d "/usr/lib/arm-linux-gnueabihf" ]; then
    sudo mkdir /usr/lib/arm-linux-gnueabihf
fi

echo "checking if Target's /lib/arm-linux-gnueabihf is mounted on /lib/arm-linux-gnueabihf ( required to link against some target .so files ) ... "

if grep -qs "/lib/arm-linux-gnueabihf" /proc/mounts; then
    echo "/lib/arm-linux-gnueabihf is mounted, skipping"
else
    echo "Mounting Target's /lib/arm-linux-gnueabihf in /lib/arm-linux-gnueabihf ..."
    sudo sshfs -o allow_other $TARGET_USER@$TARGET_IP:/lib/arm-linux-gnueabihf /lib/arm-linux-gnueabihf
fi

echo "checking if Target's /usr/lib/arm-linux-gnueabihf is mounted on /usr/lib/arm-linux-gnueabihf ( required to link against some target .so files ) ... "

if grep -qs "/usr/lib/arm-linux-gnueabihf" /proc/mounts; then
    echo "/usr/lib/arm-linux-gnueabihf is mounted, skipping"
else
    echo "Mounting Target's /usr/lib/arm-linux-gnueabihf in /usr/lib/arm-linux-gnueabihf ..."
    sudo sshfs -o allow_other $TARGET_USER@$TARGET_IP:/usr/lib/arm-linux-gnueabihf /usr/lib/arm-linux-gnueabihf
fi

export CPPFLAGS="-I${SYSROOT_DIR}/usr/include -I${SYSROOT_DIR}/usr/include/arm-linux-gnueabihf -I${SYSROOT_DIR}/opt/vc/include/ -I${SYSROOT_DIR}/opt/vc/include/interface/vcos/pthreads -I${SYSROOT_DIR}/opt/vc/include/interface/vmcs_host/linux ${AM_CPPFLAGS_OMX}"

export CXXFLAGS="$CPPFLAGS -O4"
export CFLAGS="-O4"

source $AQUABOX_ROOT/scripts/set_tools.sh
source $AQUABOX_ROOT/scripts/cmake_for_target.sh
