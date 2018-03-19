#!/bin/bash

id=`xinput list | grep "SEM USB Keyboard" | sed -n 's/.*id=\([0-9]\+\).*/\1/p' | tail -n 1`
xkbcomp -i $id second-keypad/keymap.xkb $DISPLAY
