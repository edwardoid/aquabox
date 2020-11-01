#!/bin/bash
echo 0 > /sys/class/graphics/fbcon/cursor_blink

export LD_LIBRARY_PATH=/aquabox/lib
export DBUS_SYSTEM_BUS_ADDRESS=unix:path=/var/run/dbus/system_bus_socket

LOG_FILE=/root/aquabox_stats.log

printf"\n\n\n\n\n NEW RUN\n" >> $LOG_FILE
date >> $LOG_FILE

/aquabox/bin/aquabox --service --ignore-lock --disable-access-point --ignore-networking |& tee -a $LOG_FILE
