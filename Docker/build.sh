#/bin/bash

cd /build
rm -rf *
conan install /sources --build=missing
cmake /sources
make -j8

cd /sources
GIT_HASH=$(git log --pretty=format:'%h' -n 1)
cd /build
BUILD_TIME=$(date +%s)

NAME_BASE="-${BUILD_PLATFORM}_${GIT_HASH}_${BUILD_TIME}"

CLIENT_PKG_NAME="aquabox-client$NAME_BASE"
SERVER_PKG_NAME="aquabox-server$NAME_BASE"

$CC --version > version.txt
date >> version.txt

cp aquabox-service bin/
mv config.json bin/
tar cvfz $SERVER_PKG_NAME.tar.gz version.txt lib bin

rm bin/aquabox-service

mv bin/aquabox-service .
cp aquabox bin/
cp -r assets bin/
cp -r rules bin/
cp -r devices bin/
tar cvfz $CLIENT_PKG_NAME.tar.gz version.txt lib bin
rm bin/aquabox

echo "Done"

exit