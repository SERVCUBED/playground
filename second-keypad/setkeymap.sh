#!/bin/bash

r_id=$(
    xinput list |
    sed -n 's/.*SEM.*id=\([0-9]*\).*keyboard.*/\1/p'
)
[ "$r_id" ] || exit

for r in $r_id; do
    xkbcomp -i $r keymap.xkb $DISPLAY
done
