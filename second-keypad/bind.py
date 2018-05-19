#!/usr/bin/env python
# -*- coding: utf-8 -*-

from subprocess import Popen, PIPE
import threading
import keyboard
import time

id = str(Popen(['readlink', '-f', '/dev/input/by-id/usb-SEM_USB_Keyboard-event-kbd'], stdout=PIPE).communicate()[0])[2:-3]
py = "/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py"
keystrs = {76: [py, "leftfocus"],
           75: [py, "leftkey", "ctrl+shift+Tab"],
           77: [py, "leftkey", "ctrl+Tab"],
           80: [py, "leftkey", "Down"],
           81: [py, "leftkey", "Page_Down"],
           73: [py, "leftkey", "Page_Up"],
           72: [py, "leftkey", "Up"],
           79: [py, "mumblefocus"],
           82: [py, "messengerfocus"],
           98: [py, "switchtomain"],
           # "XF86LaunchB": [py, "switchtomain"],  # Temp
           71: ["rofi", "-show", "drun", "-display-drun", "", "-fuzzy"],
           55: ["rofi", "-show", "window", "-display-window", "", "-fuzzy", "-window-format", "{w}: {t}"],
           74: ["xflock4"]}


def start(keystr):
    Popen(keystrs[keystr])


def callback(keystr):
    print("Handling", keystr)
    threading.Thread(target=start, args=[keystr]).start()


def handle_events(args):
    if not isinstance(args, keyboard.KeyboardEvent) or args.event_type != 'down' or args.device != id:
            return
    if args.scan_code in keystrs:
        print(args.scan_code, args.device, args.event_type)
        callback(args.scan_code)


def delayedsetlayout():
    time.sleep(1)
    Popen(["sh", "setkeymap.sh"])


if __name__ == '__main__':
    keyboard.hook(handle_events)
    threading.Thread(target=delayedsetlayout).start()

    #for keystr in keystrs.keys():
    #    keyboard.add_hotkey(keystr, callback, args=keystr)
    #    print("Press", keystr, "to handle ", keystrs[keystr])
    keyboard.wait()
