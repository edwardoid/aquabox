#!/bin/bash

INSTALL_DIR=/aquabox
BACKUP_DIR=$INSTALL_DIR/bin_last
SERVICE=aquabox

UPDATE=aquabox-$1.tar.gz
UPDATE_DIR=$INSTALL_DIR/update
UPDATE_DIR_TMP=$UPDATE_DIR/tmp

SERVICES=(ab_settings_provider ab_events_bus ab_devices_manager ab_rules_manager ab_network_manager aquabox-client)

mkdir -p $BACKUP_DIR
mkdir -p $UPDATE_DIR
mkdir -p $UPDATE_DIR_TMP

echo "Creating backup"
rm -rf $BACKUP_DIR
mv $INSTALL_DIR/bin $BACKUP_DIR
mkdir -p $INSTALL_DIR/bin

echo "Extracting update"
cd $UPDATE_DIR
tar -xf $UPDATE.tar -C $

if test -f "$UPDATE_DIR_TMP/pre_update.sh"
then
    source $UPDATE_DIR_TMP/pre_update.sh
else
    echo "No pre updates"
fi

for s in ${SERVICES[@]};do
    systemctl stop $s.service
done

for s in ${SERVICES[@]};do
    cp $UPDATE_DIR_TMP/bin/$s.service /etc/systemd/system/$s.service
done

systemctl daemon-deload

for s in ${SERVICES[@]};do
    systemctl enable $s.service
done


for s in ${SERVICES[@]};do
    systemctl start $s.service
done

cat $UPDATE_DIR_TMP/files.txt

for i in $(cat files.txt)
do
    if test -f "$UPDATE_DIR_TMP/$i"
    then
        echo "Replacing $i with $UPDATE_DIR_TMP/$i"
        cp $UPDATE_DIR_TMP/$i $i
    else
        echo "Update file $UPDATE_DIR_TMP/$i not found"
    fi
done

if test -f "$UPDATE_DIR_TMP/post_update.sh"
then
    echo "Doing post update"
    source $UPDATE_DIR_TMP/post_update.sh
else
    echo "No post updates provided"
fi

