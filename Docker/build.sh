#/bin/bash

BUILD_TYPE=Debug
BUILD_DIR=/build
SOURCES_DIR=/sources
TARGET_IP=""
TARGET_PASS=""
CPU_COUNT=$(nproc)

POSITIONAL_ARGS=()


_do_build=1
_do_clean=0
_pack_build=0
_onlytarget=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -b|--build_type)
            BUILD_TYPE="$2"
            shift # past argument
            shift # past value
        ;;
        -j|--parallel)
            CPU_COUNT="$2"
            shift # past argument
            shift # past value
        ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
        ;;
        --release)
            BUILD_TYPE="Release"
            shift
        ;;
        --clean)
            _do_clean=1
            shift
        ;;
        --pack)
            _pack_build=1
            shift
        ;;
        --target_only)
            _onlytarget="$2"
            shift
            shift
        ;;
        --build_dir)
            BUILD_DIR="$2"
            shift # past argument
            shift # past value
        ;;
        --sources_dir)
            SOURCES_DIR="$2"
            shift # past argument
            shift # past value
        ;;
        --conan_rebuild)
            conan cache clean "*" -s -b -d
            shift # past argument
        ;;
        --push)
            TARGET_IP="$2"
            TARGET_PASS="$3"
            shift # past argument
            shift # past value
            shift # past value
        ;;
        --help)
            echo "Usage ${0} ARGS..."
            echo "  -b, --build_type <type> : Set build type. Current [${BUILD_TYPE}]"
            echo "  -j, --parallel <number> : Set build threads count. Current [${CPU_COUNT}]"
            echo "  --release|--debug       : Set build type. Current [${BUILD_TYPE}]"
            echo "  --build_dir <dir>       : Build dir. Current [${BUILD_DIR}]"
            echo "  --sources_dir <dir>     : Sources dir. Current [${SOURCES_DIR}]"
            echo "  --clean                 : Cleans build dir. Current [${BUILD_DIR}]"
            echo "  --pack                  : Create archives with binaries"
            echo "  --target_only <name>    : Build specific target only"
            echo "  --conan_rebuild         : Clean conan build"
            echo "  --push <ip> <password>  : Pushes client binary to a target after build"
            echo "  -h, --help              : Print this help"
            shift
            _do_build=0
            _do_clean=0
        ;;
        -*|--*)
            echo "Unknown option $1"
            exit 1
        ;;
        *)
            POSITIONAL_ARGS+=("$1") # save positional arg
            shift # past argument
        ;;
    esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

function do_build {
    if [ "$SOURCES_DIR" == "$BUILD_DIR" ]; then
        echo "Build and sources directories are the same. Run build manually!"
        return 1
    fi
    mkdir -p $BUILD_DIR
    pushd $BUILD_DIR

    if [ "$BUILD_PLATFORM" = "x86_64" ]; then
        CONAN_EXTRA_ARGS="-pr default"
        CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    fi

    if [ "$BUILD_PLATFORM" = "armv6" ]; then
        CONAN_EXTRA_ARGS="-pr:h $BUILD_PLATFORM -pr:b default"
        CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    fi

    if [ "$BUILD_PLATFORM" = "armv7" ]; then
        CONAN_EXTRA_ARGS="-pr:h $BUILD_PLATFORM -pr:b default"
        CMAKE_EXTRA_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    fi

    conan install $SOURCES_DIR --output-folder=$BUILD_PLATFORM $CONAN_EXTRA_ARGS --build=missing -s build_type=$BUILD_TYPE |& tee -a build.log
    source $(find $PWD -name conanbuild.sh)
    echo "Sourcing: $(find $PWD -name conanbuild.sh)" |& tee -a build.log
    echo "New compiler: $CC and $CXX" |& tee -a build.log
    cmake $SOURCES_DIR/ -DCMAKE_TOOLCHAIN_FILE=$(find . -name conan_toolchain.cmake) $CMAKE_EXTRA_ARGS -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$PWD/lib2 -DCMAKE_INSTALL_PREFIX=$PWD/lib |& tee -a build.log
    if [ "$_onlytarget" == "" ]; then
        cmake --build . --parallel $CPU_COUNT |& tee -a build.log
    else
        cmake --build . -t $_onlytarget --parallel $CPU_COUNT |& tee -a build.log
    fi
    
    cmake --build . --parallel $CPU_COUNT -t CopyConanDependencies |& tee -a build.log
    ret=$?
    popd
    return $ret
}

function copy_file {
    echo "Copying $1 to $BUILD_DIR/bin/$1" |& tee -a build.log && \
    cp $BUILD_DIR/$1 $BUILD_DIR/bin/$1 |& tee -a build.log && \
    return $?
}

function del_file {
    echo "Deleting file $BUILD_DIR/bin/$1" |& tee -a build.log  && \
    rm $BUILD_DIR/bin/$1 |& tee -a build.log && \
    return $?
}

function copy_dir {
    echo "Copying $1 to $BUILD_DIR/bin/$1" |& tee -a build.log  && \
    cp -R $BUILD_DIR/$1 $BUILD_DIR/bin |& tee -a build.log && \
    return $?
}

function del_dir {
    echo "Deleting directory $BUILD_DIR/bin/$1" && \
    rm -rf $BUILD_DIR/bin/$1 |& tee -a build.log && \
    return $?
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
    pushd $BUILD_DIR
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

if [ $_do_clean -ne 0 ]; then
    if [ "$SOURCES_DIR" == "$BUILD_DIR" ]; then
        echo "Build and sources directories are the same. Run build manually!"
        exit 1
    fi
    rm -rf $BUILD_DIR/
fi

if [ $_do_build -ne 0 ]; then
    pushd $SOURCES_DIR
    GIT_HASH=$(git log --pretty=format:'%h' -n 1 2>/dev/null)
    GIT_BRANCH=$(git show-branch 2>/dev/null)
    popd

    BUILD_TIME=$(date +%s)

    NAME_BASE="-${BUILD_PLATFORM}_${GIT_HASH}_${BUILD_TIME}"

    CLIENT_PKG_NAME="aquabox-client$NAME_BASE"
    SERVER_PKG_NAME="aquabox-server$NAME_BASE"

    do_build
    if [ $_pack_build -ne 0 ]; then
        pushd $BUILD_DIR
        pack_server && \
        pack_client
        popd
    fi
fi