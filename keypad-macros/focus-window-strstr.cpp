#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <thread>
#include <csignal>
#include <fstream>
#include <gdk/gdkx.h>
#include <iostream>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE = 1
#include <libwnck-3.0/libwnck/libwnck.h>

// Top left corner of left portrait monitor
//const int portraittopposition[] = {1, 28};
// Top left corner of main monitor
//const int mainposition[] = {1080, 268};

// Docs: https://developer.gnome.org/libwnck/stable/

char *strtolower (char *in)
{
  for (int i = 0; i < strlen (in); i++)
    {
      if (in[i] <= 'Z' && in[i] >= 'A')
        in[i] = in[i] - ('Z' - 'z');
    }
  return in;
}

WnckWindow *find_window_strstr (const char *str)
{
  WnckScreen *s = wnck_screen_get_default ();
  wnck_screen_force_update (s);
  for (GList *windows = wnck_screen_get_windows_stacked (s);
       windows != nullptr; windows = windows->next)
    {
      auto *w = (WnckWindow *) windows->data;
      char *n = const_cast<char *>(wnck_window_get_name (w));
      //printf ("%s\n", n);
      if (strstr (strtolower (n), str) != nullptr)
        return w;
    }
  printf ("No window found containing %s\n", str);
  return nullptr;
}

int main (int argc, char *argv[])
{
  if (argc < 2)
    {
      std::cout << "Usage: focus-window-strstr <window title>";
      return EXIT_FAILURE;
    }

  gdk_init (&argc, &argv);
  WnckWindow *window = find_window_strstr (argv[1]);
  if (window == nullptr)
    return EXIT_FAILURE;
  wnck_window_activate (window, gdk_x11_get_server_time (gdk_get_default_root_window ()));

  //wnck_shutdown ();
  return EXIT_SUCCESS;
}

//bool ismainmonitorwindow(WnckWindow* w)
//{
//  int x, y;
//  wnck_window_get_client_window_geometry (w, &x, &y, nullptr, nullptr);
//  return x == mainposition[0] && y == mainposition[1];
//}

//void switch_main_window ()
//{
//  // Cycle through windows at the top left of the main monitor
//  WnckWindow* active = wnck_screen_get_active_window (default_screen);
//  auto * windows = new GList;
//  for (GList* w = wnck_screen_get_windows (default_screen); w != nullptr; w = w->next)
//    {
//      WnckWindow* wi = (WnckWindow*) w->data;
//      if (ismainmonitorwindow (wi) && !wnck_window_is_minimized (wi))
//          windows = g_list_append (windows, wi);
//    }
//
//  int i;
//  i = g_list_index (windows, active);
//  if (i >= 0)
//    i = (i == g_list_length (windows))? 0 : i;
//  else
//    i = g_list_length (windows) - 1;
//  for (; i < g_list_length (windows); i++)
//    {
//      auto* wi = (WnckWindow*) g_list_nth (windows, static_cast<guint>(i))->data;
//      printf ("%s\n", wnck_window_get_name (wi));
//      if (!wnck_window_is_most_recently_activated (wi))
//        {
//          printf ("Activating %s\n", wnck_window_get_name (wi));
//          wnck_window_activate_transient (wi, gdk_x11_get_server_time (gdk_get_default_root_window ()));
//          break;
//        }
//
//    }
//}

//void toggle_tidal_minimise()
//{
//  WnckWindow* w = find_window_strstr ("tidal");
//  if (w == nullptr)
//    {
//      printf ("null window\n");
//      return;
//    }
//  printf ("minimised: %d\n", wnck_window_is_minimized (w));
//  if (wnck_window_is_minimized (w))
//    wnck_window_unminimize (w, gdk_x11_get_server_time (gdk_get_default_root_window ()));
//  else
//    wnck_window_minimize (w);
//}
