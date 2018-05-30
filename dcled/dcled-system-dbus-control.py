#!/usr/bin/python

try:
    from gi.repository import GObject
except ImportError:
    import gobject as GObject

import sys
import dbus
import threading
from subprocess import Popen
from dbus.mainloop.glib import DBusGMainLoop

ps = None


def tshow(text):
    global ps
    if ps is not None and ps.poll() is None:
        return
    ps = Popen(["sudo", "showdcledmessage", text])


def show(text):
    threading.Thread(target=tshow, args=[text]).start()


def sysnotifications(bus, message):
    args = message.get_args_list()
    # print([arg for arg in args])
    try:
        if message.get_member() == "PropertiesChanged" and args[0] == "org.bluez.MediaTransport1" and \
                "State" in args[1] and args[1]["State"] == "active":
                # Is Bluetooth transport message
                show("BT Media Connected")

    except:
        print(str(sys.exc_info()))


DBusGMainLoop(set_as_default=True)

sysbus = dbus.SystemBus()
sysbus.add_match_string_non_blocking("eavesdrop=true, interface='org.freedesktop.DBus.Properties', "
                                     "member='PropertiesChanged'")
sysbus.add_message_filter(sysnotifications)

try:
    mainloop = GObject.MainLoop()
    mainloop.run()
except KeyboardInterrupt:
    print("Keyboard interrupt")
