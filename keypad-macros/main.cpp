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
void send_i3_focus_last(const char *cmd) {
  int fd = open ("/tmp/i3-focus-last.pipe", O_WRONLY);
  if (fd != -1)
    {
      write (fd, cmd, strlen (cmd));
      close (fd);
    }
}

void parse_keycode (int keycode)
{
  printf ("Pressed %d\n", keycode);

  switch (keycode)
    {
// KP_Divide
      case 98:
        std::system ("rofi -show drun -display-drun \uF1D8 -fuzzy");
        break;
// KP_Multiply
      case 55:
        std::system ("rofi -show window -display-window \uF1D8 -fuzzy -window-format '{w}: {t}'");
        break;
// KP_Subtract
      case 74:
        if (shifted)
          std::system ("xset dpms force off");
        else
          std::system ("cd /home/servc/git/my/playground/second-keypad/ && ./setkeymap.sh");
        break;
// KP_1
      case 79:
        std::system ("/home/servc/git/my/playground/keypad-macros/focus-window-strstr telegram");
        break;
// KP_2
      case 80:
        send_i3_focus_last ("fb\n"); // Focus bottom
        break;
// KP_3
      case 81:
        break;
// KP_4
      case 75:
        if (shifted)
          std::system ("xdotool key Alt+Left");
        else
          std::system ("i3-msg focus parent focus left focus child");
        break;
// KP_5
      case 76:
        send_i3_focus_last ("fl\n"); // Focus last
        break;
// KP_6
      case 77:
        if (shifted)
          std::system ("xdotool key Alt+Right");
        else
          std::system ("i3-msg focus parent focus right focus child");
        break;
// KP_7
      case 71:
        send_i3_focus_last ("ft\n"); // Focus top
        break;
// KP_8
      case 72:
        if (shifted)
          send_i3_focus_last ("ftoDP-2\n"); // Focus top
        else
          std::system ("i3-msg focus output DP-2");
        break;
// KP_9
      case 73:
        if (shifted)
          send_i3_focus_last ("ftoDP-0\n"); // Focus top
        else
          std::system ("i3-msg focus output DP-0");
        break;
// KP_0
      case 82:
          std::system ("xdotool click 1");
        break;
      default:
        printf ("Unhandled keycode %d", keycode);
    }
}

int main (int argc, char* argv[])
{
  const char* dev = "/dev/input/by-id/usb-SEM_USB_Keyboard-event-kbd";
  struct input_event ev;
  ssize_t n;
  int fd;

  fd = open (dev, O_RDONLY);
  if (fd == -1)
    {
      fprintf (stderr, "Cannot open %s: %s.\n", dev, strerror (errno));
      return EXIT_FAILURE;
    }

  parse_keycode (74); // Set keymap initially

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
