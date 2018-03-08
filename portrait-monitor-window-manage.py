#!/usr/bin/python2

import gtk
import wnck
import sys

# Docs: https://developer.gnome.org/libwnck/stable/

mode = ''

if len(sys.argv) > 1:
    mode = sys.argv[1]

# Ratio of top to bottom window
ratio = 0.5
ratios = {'third': 0.6, 'quarter': 0.75}
if len(sys.argv) > 2 and sys.argv[2] in ratios:
    ratio = ratios[sys.argv[2]]

# Set to bounds of desired monitor
bounds = [0, 0, 1080, 1920]
# Compensate for window manager
xoffset = -1
yoffset = -30

screen = wnck.screen_get_default()
while gtk.events_pending():
    gtk.main_iteration()

if mode == 'list':
    windows = screen.get_windows()
    for w in windows:
        print("-- " + w.get_name())
        print(w.get_client_window_geometry())
        print(w.get_state())
        print(w.is_maximized_horizontally())
    exit()

active = screen.get_active_window()

if mode == 'over':
    active.set_geometry(0, 0b1111, bounds[0] + xoffset, bounds[1] + yoffset, bounds[2], int(bounds[3] * ratio))
    exit(0)

if mode == 'under':
    active.set_geometry(0, 0b1111, bounds[0] + xoffset, bounds[1] + int(bounds[3] * ratio) + yoffset, bounds[2],
                        int(bounds[3] * (1 - ratio)) + yoffset)
    exit(0)

if mode == 'total':
    active.set_geometry(0, 0b0001, bounds[0] + xoffset, 0, 0, 0)
    active.maximize()
    exit(0)

print('No or invalid mode')


# notitles = ['Desktop', 'xfce4-panel']
#
# topbound = bounds[1]
#
# for w in windows:
#     if w.get_name() in notitles:
#         continue
#     xp, yp, widthp, heightp = w.get_client_window_geometry()
#     if not (xp == bounds[0] and xp + widthp <= bounds[3]):
#         continue
#     _topbound = yp + heightp
#     if _topbound > topbound:
#         topbound = _topbound
#     print('set topbound')
#     print(w.get_name())
#     exit()
#
# print(topbound)
# active.set_geometry(0, 15, bounds[0], topbound - 28, bounds[2], int(bounds[3]/2))
# active.maximize_horizontally()
