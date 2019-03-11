#!/bin/bash

PROJECT=$(dirname "$(readlink -f "$0")")/..

THIRD_PARTY=$PROJECT/3dp
RESTBED=$THIRD_PARTY/restbed
PLOG=$THIRD_PARTY/plog

LIB_DIR=$PROJECT/lib
mkdir -p $LIB_DIR

cd $PROJECT
git submodule update --init --recursive

echo "Building restbed..."
cd $RESTBED
git submodule update --init --recursive

echo "Building plog"
cd $PLOG
git submodule update --init --recursive
