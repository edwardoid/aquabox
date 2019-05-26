#!/bin/bash
#Copyright 2014-2015 Ionut Neicu, all rights reserved.
#This script is provided as it is, without any warranty. Use on own risk.
#This script can be used freely for cros compile on Target.

NCPU=`cat /proc/cpuinfo | grep vendor_id | wc -l`


#The Target mount options. Modify this accordingly.

TARGET_IP=edwardoid.servebeer.com
TARGET_USER=root

export ROOT_OF_CC="$PWD/../../../arm"
export SYSROOT_DIR="$ROOT_OF_CC/sysroot"
export CROSS_PATH="$ROOT_OF_CC/toolchain/bin"

mkdir -p $SYSROOT_DIR
echo "checking if Target is mounted on sysroot ... "
if grep -qs "${SYSROOT_DIR}" /proc/mounts; then
    echo "Sysroot is mounted, skipping"
else
    echo "Mounting Target in sysroot..."
    sudo sshfs -o allow_other ${TARGET_USER}@${TARGET_IP}:/ ${SYSROOT_DIR}
fi


if [  -L  "/lib/arm-linux-gnueabihf" ]; then
      sudo rm /lib/arm-linux-gnueabihf
#else
#TODO: folder must be empty before mount
fi

if [ -L "/usr/lib/arm-linux-gnueabihf" ]; then
        sudo rm /usr/lib/arm-linux-gnueabihf
#else
#TODO: folder must be emtpy before mount
fi


# Additional folders required for linker. Inside RPI, there are some .so files pointing to /lib/arm-linux-gnueabihf and /usr/lib/arm-linux-gnueabihf on target.
# You must have them on host as well. Some people reccomend editing .so file inside target and removing paths there. This should not be required with this alternate approach.

if [ ! -d "/lib/arm-linux-gnueabihf" ]; then
    sudo mkdir /lib/arm-linux-gnueabihf
#else
#TODO: folder must be empty before mount
fi

if [ ! -d "/usr/lib/arm-linux-gnueabihf" ]; then
    sudo mkdir /usr/lib/arm-linux-gnueabihf
#else
#TODO: folder must be emtpy before mount
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


export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export CC=$CROSS_PATH/arm-linux-gnueabihf-gcc
export NM=$CROSS_PATH/arm-linux-gnueabihf-nm
export LD=$CROSS_PATH/arm-linux-gnueabihf-ld
export CXX=$CROSS_PATH/arm-linux-gnueabihf-g++
export RANLIB=$CROSS_PATH/arm-linux-gnueabihf-ranlib
export AR=$CROSS_PATH/arm-linux-gnueabihf-ar
export CPP=$CROSS_PATH/arm-linux-gnueabihf-cpp
export PKG_PROG_PKG_CONFIG=$CROSS_PATH/arm-linux-gnueabihf-pkg-config
export LIBRARY_PATH=${LIBRARY_PATH}:${SYSROOT_DIR}/usr/lib/arm-linux-gnueabihf:${SYSROOT_DIR}/opt/vc/lib

echo "************* R E A D Y *************"
