#!/bin/sh
# Sets the GTK theme of firefox developer edition to Arc
# This fixes unreadable input fields when using dark GTK themes

sed -i "s/\/usr\/lib\/firefox\/firefox/firefox/" /usr/share/applications/firefox.desktop
sed -i "s/^exec/GTK_THEME=Arc exec/" /usr/bin/firefox
