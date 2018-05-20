#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time

import keyboard
import threading
from subprocess import Popen, PIPE

shifted = False
id = str(Popen(['readlink', '-f', '/dev/input/by-id/usb-SEM_USB_Keyboard-event-kbd'], stdout=PIPE).communicate()[0])[
     2:-3]
py = "/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py"
keystrs = {
    # KP_Divide
    98: [py, "switchtomain"],
    # KP_Multiply
    55: ["rofi", "-show", "window", "-display-window", "", "-fuzzy", "-window-format", "{w}: {t}"],
    # KP_Subtract
    74: ["xflock4"],
    # KP_1
    79: [py, "mumblefocus"],
    # KP_2
    80: [py, "leftkey", "Down"],
    # KP_3
    81: [py, "leftkey", "Page_Down"],
    # KP_4
    75: [py, "leftkey", "ctrl+shift+Tab"],
    # KP_5
    76: [py, "leftfocus"],
    # KP_6
    77: [py, "leftkey", "ctrl+Tab"],
    # KP_7
    71: ["rofi", "-show", "drun", "-display-drun", "", "-fuzzy"],
    # KP_8
    72: [py, "leftkey", "Up"],
    # KP_9
    73: [py, "leftkey", "Page_Up"],
    # KP_0
    82: [py, "messengerfocus"]
}
keystrs_shifted = {
    # KP_Divide
    98: [py, "loweractive"],
    # KP_Multiply
    # 55: [py, ""],
    # KP_Subtract
    # 74: [py, ""],
    # KP_1
    # 79: [py, ""],
    # KP_2
    # 80: [py, ""],
    # KP_3
    # 81: [py, ""],
    # KP_4
    75: ["xdotool", "key", "Alt+Left"],
    # KP_5
    # 76: [py, ""],
    # KP_6
    77: ["xdotool", "key", "Alt+Right"],
    # KP_7
    # 71: [py, ""],
    # KP_8
    # 72: [py, ""],
    # KP_9
    # 73: [py, ""],
    # KP_0
    # 82: [py, ""]
}


def start(keycode):
    global shifted
    Popen(keystrs_shifted[keycode] if shifted and keycode in keystrs_shifted else keystrs[keycode])


def callback(keycode):
    print("Handling", keycode, shifted and keycode in keystrs_shifted)
    threading.Thread(target=start, args=[keycode]).start()


def handle_events(args):
    if not isinstance(args, keyboard.KeyboardEvent) or args.event_type != 'down' or \
            args.device != id or args.scan_code not in keystrs:
        return
    print(args.scan_code, args.device, args.event_type, shifted)
    callback(args.scan_code)


def handle_shift(args):
    global shifted
    if args.device != id:
        return
    shifted = args.event_type == 'down'


def delayedsetlayout():
    time.sleep(1)
    Popen(["sh", "setkeymap.sh"])


if __name__ == '__main__':
    # keyboard.hook(handle_events)
    for keystr in keystrs.keys():
        keyboard.hook_key(keystr, handle_events)
        # print("Press", keystr, "to handle ", keystrs[keystr])

    keyboard.hook_key(96, handle_shift)  # KP_Enter

    threading.Thread(target=delayedsetlayout).start()

    keyboard.wait()
