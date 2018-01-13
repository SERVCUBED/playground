#!/bin/sh
# Sets the GTK theme of firefox developer edition to Arc
# This fixes unreadable input fields when using dark GTK themes

sed -i "s/\/usr\/lib\/firefox-developer-edition\/firefox/firefox-developer-edition/" /usr/share/applications/firefox-developer-edition.desktop
sed -i "s/^exec/GTK_THEME=Arc exec/" /usr/bin/firefox-developer-edition
