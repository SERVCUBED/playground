#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <linux/input.h>
#include <cstring>
#include <cstdio>
#include <thread>
#include <gdk/gdkx.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE = 1
#include <libwnck-3.0/libwnck/libwnck.h>

// Top left corner of left portrait monitor
const int portraittopposition[] = {1, 28};
// Top left corner of main monitor
const int mainposition[] = {1080, 268};

// Docs: https://developer.gnome.org/libwnck/stable/

static const char *const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
};
static bool shifted = false;
static WnckScreen* default_screen;

const char* strtolower(char* in) {
  for (int i=0; i < strlen (in); i++)
    {
      if (in[i] <= 'Z' && in[i] >= 'A')
        in[i] = in[i] - ('Z' - 'z');
    }
  return in;
}

void focus_window_strstr (const char *str)
{
  wnck_screen_force_update (default_screen);
  for (GList* windows = wnck_screen_get_windows (default_screen);
       windows != nullptr; windows = windows->next)
    {
      auto* w = (WnckWindow*) windows->data;
      if (strstr (strtolower(const_cast<char *>(wnck_window_get_name (w))), str) != nullptr)
        {
          wnck_window_activate (w, gdk_x11_get_server_time (gdk_get_default_root_window ()));
          break;
        }
    }
  //wnck_shutdown ();
}

bool ismainmonitorwindow(WnckWindow* w)
{
  int x, y;
  wnck_window_get_client_window_geometry (w, &x, &y, nullptr, nullptr);
  return x == mainposition[0] and y == mainposition[1];
}

void switch_main_window ()
{
  wnck_screen_force_update (default_screen);
  // Cycle through windows at the top left of the main monitor
  WnckWindow* active = wnck_screen_get_active_window (default_screen);
  auto * windows = new GList;
  for (GList* w = wnck_screen_get_windows (default_screen); w != nullptr; w = w->next)
    {
      WnckWindow* wi = (WnckWindow*) w->data;
      if (ismainmonitorwindow (wi) && !wnck_window_is_minimized (wi))
          windows = g_list_append (windows, wi);
    }

  int i;
  i = g_list_index (windows, active);
  if (i >= 0)
    i = (i == g_list_length (windows))? 0 : i;
  else
    i = g_list_length (windows) - 1;
  for (; i < g_list_length (windows); i++)
    {
      auto* wi = (WnckWindow*) g_list_nth (windows, static_cast<guint>(i))->data;
      if (!wnck_window_is_active (wi))
        {
          wnck_window_activate (wi, gdk_x11_get_server_time (gdk_get_default_root_window ()));
          break;
        }

    }
}

void parse_keycode (int keycode)
{
  printf ("Pressed %d", keycode);
  //GList* windows = gdk_screen_get_window_stack (default_screen);
  switch (keycode)
    {
// KP_Divide
      case 98:
          if (shifted)
            std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py loweractive");
          else
            //std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py switchtomain");
            switch_main_window ();
        break;
// KP_Multiply
      case 55:
        std::system ("rofi -show window -display-window \uF1D8 -fuzzy -window-format '{w}: {t}'");
        break;
// KP_Subtract
      case 74:
        if (shifted)
          std::system ("xflock4");
        else
          std::system ("cd /home/servc/git/my/playground/second-keypad/ && ./setkeymap.sh");
        break;
// KP_1
      case 79:
        //std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py mumblefocus");
        focus_window_strstr ("mumble -");
        break;
// KP_2
      case 80:
        std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py leftkey Down");
        break;
// KP_3
      case 81:
        std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py leftkey Page_Down");
        break;
// KP_4
      case 75:
        if (shifted)
          std::system ("xdotool key Alt+Left");
        else
          std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py leftkey ctrl+shift+Tab");
        break;
// KP_5
      case 76:
        std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py leftfocus");
        break;
// KP_6
      case 77:
        if (shifted)
          std::system ("xdotool key Alt+Right");
        else
          std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py leftkey ctrl+Tab");
        break;
// KP_7
      case 71:
        std::system ("rofi -show drun -display-drun \uF1D8 -fuzzy");
        break;
// KP_8
      case 72:
        std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py leftkey Up");
        break;
// KP_9
      case 73:
        std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py leftkey Page_Up");
        break;
// KP_0
      case 82:
        //std::system ("/home/servc/git/my/playground/second-keypad/keypad-shortcuts.py messengerfocus");
        focus_window_strstr ("messenger");
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

  //TODO remove
  setbuf (stdout, NULL);

  gdk_init (&argc, &argv);
  default_screen = wnck_screen_get_default ();

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
