#!/bin/bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

source $DIR/set_env.sh $1

sudo docker run --privileged \
                -v /var/run/dbus/system_bus_socket:/var/run/dbus/system_bus_socket \
                -v $SOURCES_DIR:/sources \
                -v $BUILD_DIR:/build \
                -it --rm $IMAGE:latest \
                /bin/bash

