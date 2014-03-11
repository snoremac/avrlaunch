#!/bin/bash

PROJECT_DIR=$(dirname $0)/..
SYMLINK=$PROJECT_DIR/tty.mcu

rm -f $SYMLINK

device=$(ls /dev/tty* |grep usbmodem)
ln -s $device $SYMLINK

echo "$SYMLINK -> $device"
