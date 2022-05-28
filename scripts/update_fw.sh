#!/bin/bash

UPDATE_ARCHIVE=$1

AB_DIR=/aquabox
AB_BACKUP_DIR=/aquabox.bak

SERVICES=(ab_settings_provider ab_events_bus ab_devices_manager ab_rules_manager ab_network_manager ab_client ab_ui)

echo "Stopping the service"
for s in ${SERVICES[@]};do
    systemctl stop $s.service
done

#TMP_DIR=$(mktemp -d -t aquabox-XXXXXXXXXX)
TMP_DIR=/install
rm -rf $TMP_DIR
mkdir -p $TMP_DIR
echo "Temporary dir: $TMP_DIR Update: $UPDATE_ARCHIVE. Extracting update archive"
tar xvf $UPDATE_ARCHIVE -C $TMP_DIR

for s in ${SERVICES[@]};do
    echo "Stopping $s"
    systemctl stop $s.service
done

for s in ${SERVICES[@]};do
    echo "Copying service file: $s"
    cp $TMP_DIR/bin/$s.service /etc/systemd/system/$s.service
done

systemctl daemon-reload

for s in ${SERVICES[@]};do
    echo "Enabling $s"
    systemctl enable $s.service
done

echo "Removing old backup"
rm -rf $AB_BACKUP_DIR

echo "Making backup"
mv $AB_DIR $AB_BACKUP_DIR

echo "Installing update"$AB_DIR/bin/config.json
mv $TMP_DIR $AB_DIR

cd $AB_DIR

echo "Restoring config file from $AB_BACKUP_DIR/bin/config.json to $AB_DIR/bin/config.json"
cp $AB_BACKUP_DIR/bin/config.json $AB_DIR/bin/config.json

echo "Restoring device files from $AB_BACKUP_DIR/data/devices to $AB_DIR/data/devices"
#cp $AB_BACKUP_DIR/data/devices/*.json $AB_DIR/data/devices/

echo "Restoring rule files from $AB_BACKUP_DIR/data/devices to $AB_DIR/data/devices"
cp $AB_BACKUP_DIR/data/rules/*.json $AB_DIR/data/rules/

echo "Version: "
cat $AB_DIR/version.txt

for s in ${SERVICES[@]};do
    echo "Starting $s"
    systemctl unmask $s.service
    systemctl start $s.service
done