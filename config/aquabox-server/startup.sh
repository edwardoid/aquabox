#!/bin/bash
echo 0 > /sys/class/graphics/fbcon/cursor_blink

export LD_LIBRARY_PATH=/aquabox/lib

/aquabox/bin/aquabox-service
