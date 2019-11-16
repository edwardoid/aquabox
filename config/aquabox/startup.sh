#!/bin/bash
echo 0 > /sys/class/graphics/fbcon/cursor_blink
export DISPLAY=:0
APP_DIR=/aquabox
BIN_DIR=$APP_DIR/bin
LOG_DIR=$APP_DIR/log
BAK_DIR=$APP_DIR/debug

mkdir -p $LOG_DIR
mkdir -p $BAK_DIR

rm -f $BAK_DIR/*
cp -r $LOG_DIR/* $BAK_DIR
rm -f $LOG_DIR/*

journalctl -b-1 -u aquabox --no-pager &> $BAK_DIR/WiFi.log
journalctl -b-1 -u NetworkManager --no-pager &> $BAK_DIR/NetworkManager.log

echo "DONE"
