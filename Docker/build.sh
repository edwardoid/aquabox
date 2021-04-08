#/bin/bash

function clean_build {
    rm build.log
    rm -rf /build/* |& tee -a build.log
}

function do_build {
    make -j6 |& tee -a build.log
    chmod +rw -R /build |& tee -a build.log
}

function prepare_build {
    conan install /sources --build=missing |& tee -a build.log
#    cmake /sources -DCMAKE_BUILD_TYPE=asan |& tee -a build.log
#    cmake /sources -DCMAKE_BUILD_TYPE=Release |& tee -a build.log
    cmake /sources -DCMAKE_BUILD_TYPE=Debug |& tee -a build.log
}

function copy_file {
    echo "Copying $1 to /build/bin/$1" |& tee -a build.log
    cp /build/$1 /build/bin/$1 |& tee -a build.log
}

function del_file {
    echo "Deleting file /build/bin/$1" |& tee -a build.log
    rm /build/bin/$1 |& tee -a build.log
}

function copy_dir {
    echo "Copying $1 to /build/bin/$1" |& tee -a build.log
    cp -R /build/$1 /build/bin |& tee -a build.log
}

function del_dir {
    echo "Deleting directory /build/bin/$1"
    rm -rf /build/bin/$1 |& tee -a build.log
}

cd /sources
GIT_HASH=$(git log --pretty=format:'%h' -n 1)
GIT_BRANCH=$(git show-branch)
cd /build
BUILD_TIME=$(date +%s)

NAME_BASE="-${BUILD_PLATFORM}_${GIT_HASH}_${BUILD_TIME}"

CLIENT_PKG_NAME="aquabox-client$NAME_BASE"
SERVER_PKG_NAME="aquabox-server$NAME_BASE"

function create_version {
    touch version.txt
    $CC --version > version.txt
    echo "Branch: $GIT_BRANCH" >> version.txt
    echo "Commit $GIT_HASH" >> version.txt
    echo "Platform: $BUILD_PLATFORM" >> version.txt
    echo "Machine: $(uname -a)" >> version.txt
    echo "Bild started at $(date)" >> version.txt
    conan --version >> version.txt
    return $?
}

function pack_server {
    tar cvfz $SERVER_PKG_NAME.tar.gz \
                    version.txt \
                    lib \
                    bin/aquabox-service \
                    bin/aquabox.service \
                    bin/config.json |& tee -a build.log
    cp $SERVER_PKG_NAME.tar.gz aquabox-server-latest.tar.gz |& tee -a build.log
}

function pack_client {
    tar cvfz $CLIENT_PKG_NAME.tar.gz \
                    version.txt \
                    lib \
                    bin/aquabox-client \
                    bin/aquabox-client.service \
                    bin/assets \
                    bin/config.json \
                    bin/devices \
                    bin/rules |& tee -a build.log
    cp $CLIENT_PKG_NAME.tar.gz aquabox-client-latest.tar.gz |& tee -a build.log
}

cd /build

clean_build && \
prepare_build && \
create_version && \
do_build && \
pack_server && \
pack_client && \
echo "Done"

#exit
