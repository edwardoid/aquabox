#!/usr/bin/bash


BUILD=41
OUT_DIR=$PWD/out


ARTIFACTORY="http://aquabox.me:8081"
PLATFORM="x86_64"
PACKAGE="aquabox-server-$BUILD.tar.gz"
URL="$ARTIFACTORY/$PLATFORM/$PACKAGE"


mkdir -p $OUT_DIR
rm -rf $OUT_DIR/*
curl -s $URL | tar -xz -C $OUT_DIR
