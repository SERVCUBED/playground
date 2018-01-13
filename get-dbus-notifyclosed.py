#!/usr/bin/python

# Handles dbus signals for when desktop notifications are closed

import dbus

try:
    from gi.repository import GObject
except ImportError:
    import gobject as GObject

from dbus.mainloop.glib import DBusGMainLoop


class MessageListener:

    def __init__(self):
        DBusGMainLoop(set_as_default=True)

        self.bus = dbus.SessionBus()
        self.proxy = self.bus.get_object('org.freedesktop.Notifications',
                                         '/org/freedesktop/Notifications')

        self.proxy.connect_to_signal('NotificationClosed',
                                     self.handle_notification)

    def handle_notification(self, *args, **kwargs):
        print(args, kwargs)


if __name__ == '__main__':
    try:
        GObject.threads_init()
        dbus.mainloop.glib.threads_init()
        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        loop = GObject.MainLoop()

        MessageListener()
        loop.run()
        dbus.set_default_main_loop(loop)
    except KeyboardInterrupt:
        print("Keyboard interrupt")
