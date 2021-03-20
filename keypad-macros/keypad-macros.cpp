#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <linux/input.h>
#include <cstring>
#include <cstdio>
#include <thread>
#include <csignal>
#include <fstream>

// Top left corner of left portrait monitor
//const int portraittopposition[] = {1, 28};
// Top left corner of main monitor
//const int mainposition[] = {1080, 268};

//static const char *const evval[3] = {
//    "RELEASED",
//    "PRESSED ",
//    "REPEATED"
//};
static bool shifted = false;

// https://github.com/SERVCUBED/i3-focus-last
char i3flpipe[100];
void send_i3_focus_last(const char *cmd) {
  int fd = open (i3flpipe, O_WRONLY);
  if (fd != -1)
    {
      write (fd, cmd, strlen (cmd));
      close (fd);
    }
  else
    fprintf (stderr, "Cannot open i3-focus-last pipe: %s\n", strerror (errno));
}

void parse_keycode (int keycode)
{
  printf ("Pressed %d\n", keycode);

  switch (keycode)
    {
#include "keypad-macros.config.h"
      default:
        printf ("Unhandled keycode %d", keycode);
    }
}

int main (int argc, char* argv[])
{
  const char* dev = "/dev/input/by-id/usb-SEM_USB_Keyboard-event-kbd";
  struct input_event ev;
  ssize_t n;
  uid_t  uid;
  int fd;

  fd = open (dev, O_RDONLY);
  if (fd == -1)
    {
      fprintf (stderr, "Cannot open %s: %s.\n", dev, strerror (errno));
      return EXIT_FAILURE;
    }

  // Grab
  if (ioctl (fd, EVIOCGRAB, 1) < 0)
    {
      fprintf (stderr, "Cannot grab %s: %s.\n", dev, strerror (errno));
    }
  //parse_keycode (74); // Set keymap initially

  // i3-focus-last
  uid = getuid();
  sprintf (i3flpipe, "/run/user/%d/i3-focus-last.sock", uid);

  while (true)
    {
      n = read (fd, &ev, sizeof ev);
      if (n == (ssize_t) -1)
        {
          printf ("Invalid read size. Not input_event.");
          if (errno == EINTR)
            continue;
          else
            break;
        }
      else if (n != sizeof ev)
        {
          errno = EIO;
          printf ("IO Error. Retrying in 5 seconds...");
          std::this_thread::sleep_for (std::chrono::seconds (5));
          continue;
        }

      if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2)
        {
          // printf ("%s 0x%04x (%d)\n", evval[ev.value], (int) ev.code, (int) ev.code);
          if (ev.code == 96)
            shifted = ev.value != 0; // RELEASED
          else if (ev.value == 1) // PRESSED
            parse_keycode ((int) ev.code);
        }
    }

  fflush (stdout);
  fprintf (stderr, "%s.\n", strerror (errno));
  return EXIT_FAILURE;
}
