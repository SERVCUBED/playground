#!/usr/bin/python2

# Keymap:
# Escape        XF86Shop        XF86LaunchB     XF86Launch7
# XF86Launch5   XF86Close       XF86Launch9     Interrobang
# XF86Launch8   XF86Launch4     XF86WebCam      Backspace
# XF86Mail      XF86Messenger   XF86Search      KP_Enter
# XF86Go        XF86Go          KP_Delete       KP_Enter

# Functions:
# Escape        Escape
# F17           switchtomain
# F18           rofi windows
# F19           Interrobang
# KP_Home       rofi
# KP_Up         leftkey Up
# KP_Page_Up    leftkey Page_Up
# F20
# KP_Left       leftkey ctrl+shift+Tab
# KP_Begin      leftfocus
# KP_Right      leftkey ctrl+Tab
# Backspace     Backspace
# KP_End        mumblefocus
# KP_Down       leftkey Down
# KP_Page_Down  leftkey Page_Down
# KP_Enter      KP_Enter
# KP_Insert     messengerfocus
# F16           Mumble push to talk

import sys
import wnck
import gtk

# Top left corner of left portrait monitor
portraittopposition = [1, 28]
# Top left corner of main monitor
mainposition = [1080, 295]

# Docs: https://developer.gnome.org/libwnck/stable/


def ismainmonitorwindow(w):
    x, y, = w.get_client_window_geometry()[:2]
    return x == mainposition[0] and y == mainposition[1]


def isleftmonitorwindow(w):
    x, y, = w.get_client_window_geometry()[:2]
    return (x == 0 if w.is_maximized() else x == portraittopposition[0]) and y == portraittopposition[1]


screen = wnck.screen_get_default()
while gtk.events_pending():
    gtk.main_iteration()
now = gtk.gdk.x11_get_server_time(gtk.gdk.get_default_root_window())


if len(sys.argv) < 2:
    exit(1)
arg = sys.argv[1]

if arg == "messengerfocus":
    # Focus messenger window
    for w in screen.get_windows():
        n = w.get_name().lower()
        # print(n)
        if "messenger" in n or "message" in n:
            w.activate(now)
            break
elif arg == "mumblefocus":
    # Focus messenger window
    for w in screen.get_windows():
        n = w.get_name()
        # print(n)
        if "Mumble --" in n:
            w.activate(now)
            break
elif arg == "leftfocus":
    # Focus the window on the leftmost (portrait) monitor
    for w in screen.get_windows():
        if isleftmonitorwindow(w):
            w.activate(now)
            break
elif arg == "leftkey" and len(sys.argv) == 3:
    # Send a key to the top window on my left (portrait) monitor
    key = sys.argv[2]

    for w in screen.get_windows():
        if isleftmonitorwindow(w):
            from subprocess import Popen
            Popen(['xdotool', 'key', '--window', str(w.get_xid()), key])
            break
elif arg == "switchtomain":
    # Cycle through windows at the top left of the main monitor
    active = screen.get_active_window()
    ws = [w for w in screen.get_windows() if ismainmonitorwindow(w)]

    if active in ws:
        i = ws.index(active)
        starti = 0 if i == len(ws) - 1 else i
    else:
        starti = len(ws) - 1
    for w in ws[starti:]:
        if not w.is_active():
            w.activate(now)
            break
elif arg == "fullscreen":
    w = screen.get_active_window()
    w.set_fullscreen(not w.is_fullscreen())
