#!/usr/bin/env python3

from subprocess import Popen
import sys
import threading

import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

onlySummarySenders = sys.argv[1:]

ps = None


def tshow(text):
    global ps
    if ps is not None and ps.poll() is None:
        return
    # Copy showdcledmessage to PATH and set NOPASSWD in /etc/sudoers
    # (Make sure to change ownership of file to avoid unwanted changes which can be run as root)
    ps = Popen(["sudo", "showdcledmessage", text])


def show(text):
    threading.Thread(target=tshow, args=[text]).start()


def notifications(bus, message):
    args = message.get_args_list()
    # print([arg for arg in args])
    try:
        if message.get_member() == "Notify" and args[3] != "":
            # Is notification
            print("Is Notify")
            show(args[3] + ((" - " + args[4] if len(args[4]) < 50 else args[4][:50] + "...") if (args[0] not in onlySummarySenders and args[4] != "") else ""))
        elif message.get_member() == "PropertiesChanged":
            # Is mpris property change
            if args[0] == "org.mpris.MediaPlayer2.Player":
                if "Metadata" in args[1] and "xesam:title" in args[1]["Metadata"]:
                    print("Is MPRIS track title PropertiesChanged")
                    text = args[1]["Metadata"]["xesam:title"]
                    if "xesam:artist" in args[1]["Metadata"] and args[1]["Metadata"]["xesam:artist"] != "":
                        text += " - " + args[1]["Metadata"]["xesam:artist"][0]
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




bus = dbus.SessionBus(mainloop=DBusGMainLoop(set_as_default=True))
bus.add_match_string_non_blocking("eavesdrop=true, interface='org.freedesktop.Notifications', member='Notify'")
bus.add_match_string_non_blocking("eavesdrop=true, interface='org.freedesktop.DBus.Properties', "
                                  "member='PropertiesChanged'")
bus.add_message_filter(notifications)

try:
    mainloop = GLib.MainLoop()
    mainloop.run()
except KeyboardInterrupt:
    print("Keyboard interrupt")
