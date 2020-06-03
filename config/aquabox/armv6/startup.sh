#!/bin/bash
echo 0 > /sys/class/graphics/fbcon/cursor_blink

export LD_LIBRARY_PATH=/aquabox/lib
export DBUS_SYSTEM_BUS_ADDRESS=unix:path=/var/run/dbus/system_bus_socket

modprobe fbtft_device name=adafruit18 gpios=reset:27,dc:22,led:5,cs:17 fps=30 rotate=90 busnum=0

/aquabox/bin/aquabox --service --ignore-lock --disable-access-point
