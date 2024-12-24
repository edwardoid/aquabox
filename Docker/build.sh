#/bin/bash

USABLE_CPU_COUNT=$(nproc)
J_ARG=$(( $USABLE_CPU_COUNT < 0 ? 1 : $USABLE_CPU_COUNT ))
export CONAN_CPU_COUNT=$J_ARG

function clean_build {
    touch build.log
    rm build.log && \
    rm -rf /build/* |& tee -a build.log
    return $?
}

function prepare_env {
    pip install conan -U
    return $?
}

function do_build {
    pushd /build
    CPU_COUNT=$(grep -c ^processor /proc/cpuinfo)
    cmake --build . --parallel $CPU_COUNT |& tee -a build.log
    ret=$?
    chmod +rw -R /build |& tee -a build.log
    for dep in $(find $PWD/conan_out/ -name *.so*) ; do    #print the new array 
        cp "$value" $PWD/lib 
    done
    popd
    return $ret
}

function prepare_build {
    pushd /build
    if [ "$BUILD_PLATFORM" = "x86_64" ]; then
        export CONAN_EXTRA_ARGS="-pr default" --deployer=direct_deploy --deployer-folder=$PWD/my_deployer
        export CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=Release"
    fi

    if [ "$BUILD_PLATFORM" = "armv6" ]; then
        export CONAN_EXTRA_ARGS="-pr:h $BUILD_PLATFORM -pr:b default"
        export CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=Release"
    fi

    if [ "$BUILD_PLATFORM" = "armv7" ]; then
        #export CONAN_EXTRA_ARGS="-pr:h $BUILD_PLATFORM -pr:b default"
        #export CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_TOOLCHAIN_FILE=/sources/toolchain_armv7.cmake"

        export CONAN_EXTRA_ARGS="-pr:h $BUILD_PLATFORM -pr:b default"
        export CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=Release"
    fi

    conan install /sources --output-folder=$BUILD_PLATFORM $CONAN_EXTRA_ARGS --build=missing |& tee -a build.log
    source $(find $PWD -name conanbuild.sh)
    echo "Sourcing: $(find $PWD -name conanbuild.sh)" |& tee -a build.log
    echo "New compiler: $CC and $CXX" |& tee -a build.log
    cmake /sources/ -DCMAKE_TOOLCHAIN_FILE=$(find . -name conan_toolchain.cmake) $CMAKE_EXTRA_ARGS -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$PWD/lib2 -DCMAKE_INSTALL_PREFIX=$PWD/lib |& tee -a build.log
    CPU_COUNT=$(grep -c ^processor /proc/cpuinfo)
    cmake --build . --parallel $CPU_COUNT |& tee -a build.log
    cmake --build . --parallel $CPU_COUNT -t CopyConanDependencies |& tee -a build.log
    ret=$?
    popd
    return $ret
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
    pushd /build
    touch version.txt
    echo "Branch: $GIT_BRANCH" > version.txt
    echo "Commit $GIT_HASH" >> version.txt
    echo "Platform: $BUILD_PLATFORM" >> version.txt
    echo "Machine: $(uname -a)" >> version.txt
    echo "Build started at $(date)" >> version.txt
    conan --version >> version.txt
    ret=$?
    popd
    return $ret
}

function pack_server {
    tar cvfz $SERVER_PKG_NAME.tar.gz \
                    lib \
                    bin/aquabox-server \
                    bin/aquabox-server.service \
                    bin/config.json |& tee -a build.log && \
    cp $SERVER_PKG_NAME.tar.gz aquabox-server-latest.tar.gz |& tee -a build.log && \
    return $?
}

function pack_client {
    pushd /build
    tar cvfz $CLIENT_PKG_NAME.tar.gz \
                    lib \
                    data \
                    bin/ab_client \
                    bin/ab_client.service \
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
                    data/rules |& tee -a build.log
    cp $CLIENT_PKG_NAME.tar.gz aquabox-client-latest.tar.gz |& tee -a build.log
    ret=$?
    popd
    return $ret
}

mkdir -p /build
pushd /build

create_version
clean_build
prepare_build
do_build
pack_server && \
pack_client && \
echo "Done"

popd
#exit
