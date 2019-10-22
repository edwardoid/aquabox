#/bin/bash

echo "Required CMAKE is 3.7.2"
export CMAKE=$HOME/cmake-3.7.2/bin/cmake
export SRC_DIR=$PWD/../..
echo "Toolchain should be installed in /usr"
source $SRC_DIR/setup-arm-dev-env.sh
#rm -rf *
$CMAKE $SRC_DIR -DCMAKE_BUILD_TYPE=Release -DTARGET_DEVICE=YES -DCMAKE_TOOLCHAIN_FILE=$SRC_DIR/arm-gcc-toolchain.cmake
make -j8
#cp -r $SRC_DIR/cfg $PWD/devices
#cp $SRC_DIR/config.json $PWD/config.json
#cp startup.sh $PWD/startup.sh

