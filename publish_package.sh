#!/bin/bash

BUILD_DIR=$1
INSTALL_DIR=$BUILD_DIR/install
HASH=$(git rev-parse --short HEAD)
DATE=$(date +%s)

cd $BUILD_DIR

make

rm -rf $INSTALL_DIR
mkdir -p $INSTALL_DIR/aquabox/bin

cp $BUILD_DIR/aquabox $INSTALL_DIR/aquabox/bin
cp -r $BUILD_DIR/assets $INSTALL_DIR/aquabox/bin/assets
cp $BUILD_DIR/config.json $INSTALL_DIR/aquabox/config.json

echo $INSTALL_DIR
echo "find $INSTALL_DIR -type f | sed 's;$INSTALL_DIR/;;' > $BUILD_DIR/files.txt"
find install -type f | sed 's;install/;;' > $BUILD_DIR/files.txt
mv $BUILD_DIR/files.txt $INSTALL_DIR/files.txt
tar cvf aquabox-$HASH-$DATE.tar.gz install --transform 's|install||'

