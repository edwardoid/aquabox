#!/bin/bash

UPDATE_ARCHIVE=$1

AB_DIR=/aquabox
AB_BACKUP_DIR=/aquabox.bak

echo "Stopping the service"
systemctl stop aquabox

TMP_DIR=$(mktemp -d -t aquabox-XXXXXXXXXX)
echo "Temporary dir: $TMP_DIR Update: $UPDATE_ARCHIVE. Extracting update archive"
tar xvf $UPDATE_ARCHIVE -C $TMP_DIR

function install_service {
    if test -f "$TMP_DIR/bin/$1.service"; then
        systemctl stop $1
        echo "Updating service"
        cp $TMP_DIR/bin/$1.service /etc/systemd/system/$1.service
        systemctl daemon-reload
    fi
}

install_service aquabox-client
install_service ab_rules_manager
install_service ab_devices_manager
install_service ab_network_manager
install_service ab_events_bus
install_service ab_settings_provider

systemctl start ab_settings_provider
systemctl start ab_events_bus
systemctl start ab_network_manager
systemctl start ab_devices_manager
systemctl start ab_rules_manager
systemctl start aquabox-client

if test -f "$TMP_DIR/bin/aquabox-client.service"; then
    echo "Updating service"
    cp $TMP_DIR/bin/aquabox-client.service /etc/systemd/system/aquabox.service
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
systemctl restart aquabox-client

echo "Checking service"
systemctl status aquabox-client

journalctl -f -u aquabox-client

echo "Version: "
cat $AB_DIR/version.txt