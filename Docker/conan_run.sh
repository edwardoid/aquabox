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
    export DOCKER_EXTRA_ARGS_INT="-p 1213:1213 -p 1214:1214 -p 5555:5555 -p 5556:5556 -p 8974:8974 -p 2159:2159 -p 1800:1800 -p 1883:1883 --hostname $1 -v /var/run/dbus/system_bus_socket:/var/run/dbus/system_bus_socket"
elif [ "$1" = "armv6" ]; then
    export DOCKER_EXTRA_ARGS_INT="-v $SOURCES_DIR/Docker/conan_armv6:/root/.conan/profiles/armv6 -e LD_FLAGS='-lpthread -lm'"
else
    export DOCKER_EXTRA_ARGS_INT="-v $SOURCES_DIR/Docker/conan_armv7:/root/.conan/profiles/armv7"
fi

if [ -v DOCKER_EXTRA_ARGS ]; then
    export DOCKER_EXTRA_ARGS_INT="$DOCKER_EXTRA_ARGS $DOCKER_EXTRA_ARGS_INT"
fi

if [ -n "$PERMANENT_CONAN" ]; then
  mkdir -p $DIR/../conan_data_$1
  export DOCKER_EXTRA_ARGS_INT="-v $DIR/../conan_data_$1:/root/.conan/data  $DOCKER_EXTRA_ARGS_INT"
fi

echo "Running conan with args: $DOCKER_EXTRA_ARGS_INT"

docker run --privileged \
           -v $SOURCES_DIR:/sources \
           -v $BUILD_DIR:/build \
           -v /etc/timezone:/etc/timezone:ro \
           -v /etc/localtime:/etc/localtime:ro \
           $DOCKER_EXTRA_ARGS_INT \
           -e BUILD_PLATFORM=$1 \
           -e CONAN_CPU_COUNT=$CPU_COUNT \
           -it --rm $IMAGE:latest \
            /bin/bash

