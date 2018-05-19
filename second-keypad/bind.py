#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import gi

gi.require_version('Gtk', '3.0')
gi.require_version('Keybinder', '3.0')

from gi.repository import Gtk
from gi.repository import Keybinder
from subprocess import Popen
import threading

py = "/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py"
keystrs = {"XF86Launch4": [py, "leftfocus"],
           "XF86Launch8": [py, "leftkey", "ctrl+shift+Tab"],
           "XF86WebCam": [py, "leftkey", "ctrl+Tab"],
           "XF86Messenger": [py, "leftkey", "Down"],
           "XF86Search": [py, "leftkey", "Page_Down"],
           "XF86Launch9": [py, "leftkey", "Page_Up"],
           "XF86Close": [py, "leftkey", "Up"],
           "XF86Mail": [py, "mumblefocus"],
           "XF86Go": [py, "messengerfocus"],
           "XF86Shop": [py, "switchtomain"],
           # "XF86LaunchB": [py, "switchtomain"],  # Temp
           "XF86Launch5": ["rofi", "-show", "drun", "-display-drun", "", "-fuzzy"],
           "XF86LaunchB": ["rofi", "-show", "window", "-display-window", "", "-fuzzy", "-window-format", "{w}: {t}"],
           "XF86Launch7": ["xflock4"]}


def start(keystr):
    Popen(keystrs[keystr])


def callback(keystr, user_data):
    print("Handling", keystr, user_data)
    print("Event time:", Keybinder.get_current_event_time())
    threading.Thread(target=start, args=[keystr]).start()


if __name__ == '__main__':
    Popen(["sh", "setkeymap.sh"])
    Keybinder.init()
    Keybinder.set_use_cooked_accelerators(True)
    for keystr in keystrs.keys():
        Keybinder.bind(keystr, callback, "Keystring %s (user data)" % keystr)
        print("Press", keystr, "to handle ", keystrs[keystr])
    Gtk.main()
