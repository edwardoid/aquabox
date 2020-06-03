#!/bin/bash

HOST=$1
PASS=$2
PACKAGE=aquabox-latest.tar.gz

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

echo "Copying $DIR/update_fw.sh"
sshpass -p $PASS scp $DIR/update_fw.sh root@$HOST:update_fw.sh
echo "Copying $PWD/$PACKAGE"
sshpass -p $PASS scp $PWD/$PACKAGE root@$HOST:$PACKAGE

sshpass -p $PASS ssh root@$HOST bash +x update_fw.sh $PACKAGE
sshpass -p $PASS ssh root@$HOST rm update_fw.sh
sshpass -p $PASS ssh root@$HOST rm $PACKAGE
