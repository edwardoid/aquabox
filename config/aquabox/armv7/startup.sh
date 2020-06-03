#!/bin/bash
echo 0 > /sys/class/graphics/fbcon/cursor_blink

export LD_LIBRARY_PATH=/aquabox/lib
export DBUS_SYSTEM_BUS_ADDRESS=unix:path=/var/run/dbus/system_bus_socket

/aquabox/bin/aquabox --service --ignore-lock --disable-access-point
