#!/bin/bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

source $DIR/set_env.sh $1

if [ "$1" = "x86_64" ]; then
    export DOCKER_EXTRA_ARGS="--hostname $1"
elif [ "$1" = "armv6" ]; then
    export DOCKER_EXTRA_ARGS="-v $SOURCES_DIR/Docker/conan_armv6:/root/.conan/profiles/armv6"
else
    export DOCKER_EXTRA_ARGS="-v $SOURCES_DIR/Docker/conan_armv7:/root/.conan/profiles/armv7"
fi

docker run -v $SOURCES_DIR:/sources \
				-v $BUILD_DIR:/build \
        $DOCKER_EXTRA_ARGS \
        -e BUILD_PLATFORM=$1 \
        -e CONAN_CPU_COUNT=4 \
				--rm $IMAGE:latest \
				/bin/bash /sources/Docker/build.sh |& tee $LOG_FILE
