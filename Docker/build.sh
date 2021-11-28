#/bin/bash

USABLE_CPU_COUNT=$(($CPU_COUNT - 2))
J_ARG=$(( $USABLE_CPU_COUNT < 0 ? 1 : $USABLE_CPU_COUNT ))
export CONAN_CPU_COUNT=$J_ARG

function clean_build {
    touch build.log
    rm build.log && \
    rm -rf /build/* |& tee -a build.log
    return $?
}

function do_build {
    CPU_COUNT=$(grep -c ^processor /proc/cpuinfo)
    make -j$J_ARG |& tee -a build.log && \
    chmod +rw -R /build |& tee -a build.log
    return $?
}

function prepare_build {
    if [ "$BUILD_PLATFORM" = "x86_64" ]; then
        export CONAN_EXTRA_ARGS="-pr default"
        export CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=Debug"
    fi

    if [ "$BUILD_PLATFORM" = "armv6" ]; then
        export CONAN_EXTRA_ARGS="-pr:h $BUILD_PLATFORM -pr:b default"
        export CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/sources/toolchain_armv6.cmake"
    fi

    if [ "$BUILD_PLATFORM" = "armv7" ]; then
        export CONAN_EXTRA_ARGS=-"pr:h $BUILD_PLATFORM -pr:b default"
        export CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/sources/toolchain_armv7.cmake"
    fi

    conan install /sources $CONAN_EXTRA_ARGS --build missing |& tee -a build.log  && \
    cmake /sources $CMAKE_EXTRA_ARGS |& tee -a build.log && \
    return $?
}

function copy_file {
    echo "Copying $1 to /build/bin/$1" |& tee -a build.log && \
    cp /build/$1 /build/bin/$1 |& tee -a build.log && \
    return $?
}

function del_file {
    echo "Deleting file /build/bin/$1" |& tee -a build.log  && \
    rm /build/bin/$1 |& tee -a build.log && \
    return $?
}

function copy_dir {
    echo "Copying $1 to /build/bin/$1" |& tee -a build.log  && \
    cp -R /build/$1 /build/bin |& tee -a build.log && \
    return $?
}

function del_dir {
    echo "Deleting directory /build/bin/$1" && \
    rm -rf /build/bin/$1 |& tee -a build.log && \
    return $?
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
    touch version.txt && \
    echo "Branch: $GIT_BRANCH" >> version.txt && \
    echo "Commit $GIT_HASH" >> version.txt && \
    echo "Platform: $BUILD_PLATFORM" >> version.txt && \
    echo "Machine: $(uname -a)" >> version.txt && \
    echo "Bild started at $(date)" >> version.txt && \
    conan --version >> version.txt && \
    return $?
}

function pack_server {
    tar cvfz $SERVER_PKG_NAME.tar.gz \
                    version.txt \
                    lib \
                    bin/aquabox-server \
                    bin/aquabox-server.service \
                    bin/config.json |& tee -a build.log && \
    cp $SERVER_PKG_NAME.tar.gz aquabox-server-latest.tar.gz |& tee -a build.log && \
    return $?
}

function pack_client {
    tar cvfz $CLIENT_PKG_NAME.tar.gz \
                    version.txt \
                    lib \
                    data \
                    bin/aquabox-client \
                    bin/aquabox-client.service \
                    bin/ab_devices_manager \
                    bin/ab_devices_manager.service \
                    bin/ab_events_bus \
                    bin/ab_events_bus.service \
                    bin/ab_network_manager \
                    bin/ab_network_manager.service \
                    bin/ab_rules_manager \
                    bin/ab_rules_manager.service \
                    bin/ab_settings_provider \
                    bin/ab_settings_provider.service \
                    bin/ab_ui \
                    bin/ab_ui.service \
                    data/assets \
                    bin/config.json \
                    data/devices \
                    data/rules |& tee -a build.log && \
    cp $CLIENT_PKG_NAME.tar.gz aquabox-client-latest.tar.gz |& tee -a build.log && \
    return $?
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
