#!/usr/bin/python

try:
    from gi.repository import GObject
except ImportError:
    import gobject as GObject

import sys
import dbus
from subprocess import Popen
from dbus.mainloop.glib import DBusGMainLoop


ps = None


def show(text):
    global ps
    if ps is not None and ps.returncode is not None:
        return
    ps = Popen(["sudo", "showdcledmessage", text])


def notifications(bus, message):
    args = message.get_args_list()
    # print([arg for arg in args])
    try:
        if message.get_member() == "Notify" and args[4] != "":
            # Is notification
            print("Is Notify")
            show(args[3] + " - " + args[4] if len(args[4]) < 50 else args[4][:50] + "...")
        elif message.get_member() == "PropertiesChanged":
            # Is mpris property change
            if args[0] == "org.mpris.MediaPlayer2.Player":
                if "Metadata" in args[1] and "xesam:title" in args[1]["Metadata"]:
                    print("Is MPRIS track title PropertiesChanged")
                    text = args[1]["Metadata"]["xesam:title"]
                    if "xesam:artist" in args[1]["Metadata"] and args[1]["Metadata"]["xesam:artist"] != "":
                        text += " - " + args[1]["Metadata"]["xesam:artist"]
                    show("Now playing: " + text)

                # Spec:
                # [dbus.String('org.mpris.MediaPlayer2.Player'),
                # dbus.Dictionary({
                #     dbus.String('Metadata'): dbus.Dictionary({
                #         dbus.String('mpris:artUrl'): dbus.String(
                #             'https://resources.tidal.com/images/...jpg',
                #             variant_level=1),
                #         dbus.String('mpris:length'): dbus.UInt32(190000000, variant_level=1),
                #         dbus.String('xesam:album'): dbus.String('', variant_level=1),
                #         dbus.String('xesam:artist'): dbus.String('Title', variant_level=1),
                #         dbus.String('xesam:title'): dbus.String('Artist', variant_level=1)
                #     }, signature=dbus.Signature('sv'), variant_level=1)
                # },
                # signature=dbus.Signature('sv')),
                # dbus.Array([], signature=dbus.Signature('s'))]

    except:
        print(str(sys.exc_info()))


DBusGMainLoop(set_as_default=True)

bus = dbus.SessionBus()
bus.add_match_string_non_blocking("eavesdrop=true, interface='org.freedesktop.Notifications', member='Notify'")
bus.add_match_string_non_blocking("eavesdrop=true, interface='org.freedesktop.DBus.Properties', "
                                  "member='PropertiesChanged'")
bus.add_message_filter(notifications)

try:
    mainloop = GObject.MainLoop()
    mainloop.run()
except KeyboardInterrupt:
    print("Keyboard interrupt")
