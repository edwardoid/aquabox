#/bin/bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

export SOURCES_DIR=$DIR/..
export BUILD_DIR=$SOURCES_DIR/build/$1
export LOG_FILE=$SOURCES_DIR/build/$1.log

echo "Sources located in $SOURCES_DIR"
export mkdir -p $BUILD_DIR
echo "Build will be done in $BUILD_DIR"
echo "Logs will be $LOG_FILE"


export IMAGE=conan_$1

sudo docker build -t $IMAGE - < $DIR/Dockerfile.$1
