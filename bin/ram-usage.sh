#!/bin/bash -eu

FIRMWARE=$1

avr-nm --print-size --size-sort --radix=d $FIRMWARE \
  | awk '/ [DdBb] / {print $1, $2, $3, $4}'
