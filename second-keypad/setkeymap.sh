#!/bin/bash

do_for_dev(){
	local dev_re=$1
	shift
	local dev_cmd=$@
	
	r_id=$(
		xinput list |
		sed -n "$dev_re"
	)
	[ "$r_id" ] || { echo "No device matching $dev_re"; return; }

	for r in $r_id; do
		echo "${dev_cmd@P}"
		eval "${dev_cmd}"
	done
}

do_for_dev 's/.*SEM.*id=\([0-9]*\).*keyboard.*/\1/p' xkbcomp -i '$r' keymap.xkb $DISPLAY
do_for_dev 's/.*K70.*id=\([0-9]*\).*keyboard.*/\1/p' setxkbmap -device '$r' -option caps:escape
# caps:super

setnumlock 0 "SEM USB Keyboard"

xset r rate 400 50
#setxkbmap -option caps:super
#killall xcape 2>/dev/null ; xcape -e 'Super_L=Escape'

