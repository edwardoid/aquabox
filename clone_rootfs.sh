#/bin/bash

export HOST=edwardoid.servebeer.com
export USER=root

export ROOTFS=$PWD/rootfs

rsync -rlvp --safe-links --delete-after $USER@$HOST:/{lib,usr,etc,opt} $ROOTFS

