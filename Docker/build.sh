#/bin/bash

cd /build
rm -rf *
conan install /sources --build=missing
cmake /sources
make -j8

echo "Build done"

function copy_file {
    echo "Copying $1 to /build/bin/$1"
    cp /build/$1 /build/bin/$1
}

function del_file {
    echo "Deleting file /build/bin/$1"
    rm /build/bin/$1
}

function copy_dir {
    echo "Copying $1 to /build/bin/$1"
    cp -R /build/$1 /build/bin
}

function del_dir {
    echo "Deleting directory /build/bin/$1"
    rm -rf /build/bin/$1
}

cd /sources
GIT_HASH=$(git log --pretty=format:'%h' -n 1)
cd /build
BUILD_TIME=$(date +%s)

NAME_BASE="-${BUILD_PLATFORM}_${GIT_HASH}_${BUILD_TIME}"

CLIENT_PKG_NAME="aquabox-client$NAME_BASE"
SERVER_PKG_NAME="aquabox-server$NAME_BASE"

$CC --version > version.txt
date >> version.txt

copy_file aquabox-service
copy_file config.json
tar cvfz $SERVER_PKG_NAME.tar.gz version.txt lib bin
cp $SERVER_PKG_NAME.tar.gz aquabox-server-latest.tar.gz

del_file aquabox-service

echo "Copy"
copy_file aquabox
copy_file aquabox.service
copy_file startup.sh
copy_dir assets
copy_dir rules
copy_dir devices

tar cvfz $CLIENT_PKG_NAME.tar.gz version.txt lib bin
cp $CLIENT_PKG_NAME.tar.gz aquabox-latest.tar.gz

chmod +rw -R *

echo "Done"

exit
