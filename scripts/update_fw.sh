#!/bin/bash

UPDATE_ARCHIVE=$1

AB_DIR=/aquabox
AB_BACKUP_DIR=/aquabox.bak

echo "Stopping the service"
systemctl stop aquabox

TMP_DIR=$(mktemp -d -t aquabox-XXXXXXXXXX)
echo "Temporary dir: $TMP_DIR Update: $UPDATE_ARCHIVE. Extracting update archive"
tar xvf $UPDATE_ARCHIVE -C $TMP_DIR

if test -f "$TMP_DIR/bin/aquabox.service"; then
    echo "Updating service"
    cp $TMP_DIR/bin/aquabox.service /etc/systemd/system/aquabox.service
    systemctl daemon-reload
fi

echo "Removing old backup"
rm -rf $AB_BACKUP_DIR

echo "Making backup"
mv $AB_DIR $AB_BACKUP_DIR

echo "Installing update"$AB_DIR/bin/config.json
mv $TMP_DIR $AB_DIR

cd $AB_DIR

echo "Restoring config file from $AB_BACKUP_DIR/bin/config.json to $AB_DIR/bin/config.json"
cp $AB_BACKUP_DIR/bin/config.json $AB_DIR/bin/config.json

echo "Restarting service"
systemctl restart aquabox

echo "Checking service"
systemctl status aquabox

echo "Version: "
cat $AB_DIR/version.txt